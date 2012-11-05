#include "samson/worker/SamsonWorkerController.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <utility>   // std::pair<>

#include "au/log/LogMain.h"

#include "samson/common/Logs.h"
#include "samson/common/common.h"
#include "samson/common/KVRange.h"
#include "samson/stream/BlockManager.h"
#include "zoo/common.h"

namespace samson {
  SamsonWorkerController::SamsonWorkerController(au::zoo::Connection *zoo_connection, int port, int port_web) :
  token_("SamsonWorkerController") {
    // zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
    
    // Keep a pointer to the connection
    zoo_connection_ = zoo_connection;
    
    // Make sure basic folders are created
    
    AU_M( logs.worker_controller, ("Creating basic folders in zk. Just in case I am the first one..."));
    zoo_connection_->Create("/samson");
    zoo_connection_->Create("/samson/workers");
    zoo_connection_->Create("/samson/workers_blocks");
    zoo_connection_->Create("/samson/new_worker", 0, "0", 1); // Node used to notify cluster leather about new workers
    
    // Keep information about this worker
    port_ = port;
    port_web_ = port_web;
    
    // Counter to assign number to created blocks
    block_id_counter_ = 1;
    
    // Fill basic information for this worker
    worker_info_.set_host(get_local_ip());
    worker_info_.set_port(port_);
    worker_info_.set_port_web(port_web_);
    worker_info_.set_cores(au::Singleton<SamsonSetup>::shared()->GetUInt64("general.num_processess"));
    worker_info_.set_memory(au::Singleton<SamsonSetup>::shared()->GetUInt64("general.memory"));
    worker_info_.set_last_commit_id(0);
    
  }
  
  int SamsonWorkerController::init() {
    
    AU_M( logs.worker_controller, ("SamsonWorkerController constructor"));
    
    // Create ephemeral node with my information
    node_worker_ = NODE_WORKER_BASE;
    int rc = zoo_connection_->Create(node_worker_, ZOO_SEQUENCE | ZOO_EPHEMERAL, &worker_info_);
    if (rc) {
      LM_W(("Not possible to create ephemeral worker node at %s (%s)"
            , node_worker_.c_str()
            , au::zoo::str_error(rc).c_str()));
      return rc;
    }
    
    AU_M( logs.worker_controller, ("Ephemeral node created at %s", node_worker_.c_str()));
    
    // Get my assigned worker_id
    worker_id_ = worker_from_node(node_worker_);
    AU_M( logs.worker_controller, ("Assigned Worker id: %lu", worker_id_));

    // General function to check everything
    rc = Review();
    if (rc) {
      LM_W(("Not possible to check samson worker controller %s (%s)"
            , node_worker_.c_str()
            , au::zoo::str_error(rc).c_str()));
      return rc;
    }

    // If I am not the cluster, notify to the leather "touching" node /samson/new_worker
    if (!cluster_leader_) {
      rc = zoo_connection_->Set("/samson/new_worker", "0", 1);
      if (rc) {
        LM_W(("Error touching node /samson/new_worker (%s)", au::zoo::str_error(rc).c_str()));
        return rc;
      }
    }
    
    // Recover cluster information ( independently if I am or not the leader of the cluster )
    rc = RecoverClusterInfo();
    if (rc) {
      LM_W(("Not possible to recover cluster information %s (%s)"
            , node_worker_.c_str()
            , au::zoo::str_error(rc).c_str()));
      return rc;
    }
    
    return 0; // OK
  }
  
  int SamsonWorkerController::RecoverClusterInfo() {
    
    au::TokenTaker tt(&token_);
    
    // Recover cluster information and add watch for updates
    AU_M( logs.worker_controller, ("Recovering cluster information"));
    
    au::SharedPointer<gpb::ClusterInfo> cluster_info(new gpb::ClusterInfo());
    
    Stat stat;
    int rc = zoo_connection_->Get("/samson/cluster", engine_id(), cluster_info.shared_object(), &stat);
    if (rc) {
      return rc;
    }
    
    if ((cluster_info_ == NULL) || (cluster_info->version() > cluster_info_->version())) {
      // New version, so replace current version of the cluster and alert the system
      // Note that if there is no new version, it is not necessary to aler this worker
      cluster_info_ = cluster_info;
      
      // Notify to everybody that cluster_info changed
      engine::Engine::shared()->notify(new engine::Notification("notification_cluster_info_changed_in_worker"));
    }
    
    return 0; // OK in both cases ( new or same version )
  }
  
  void SamsonWorkerController::notify(engine::Notification *notification) {
    au::TokenTaker tt(&token_);
    
    if (notification->isName(notification_zoo_watcher)) {
      
      std::string path = notification->environment().Get("path", "");
      int type = notification->environment().Get("type", (int) -1 );
      
      if ( !au::CheckIfStringsBeginWith(path, "/samson/") )
        return;
      
      AU_M( logs.worker_controller, ("SamsonWorkerController watch %s", path.c_str()));

      // Updates on cluster information
      if( path == "/samson/cluster")
      {
        int rc = RecoverClusterInfo(); // Rest of watchers are related with workers up or down
        if (rc) {
          LM_W(("Error recovering cluster information (error %s)", au::zoo::str_error(rc).c_str()));
        }
        return;
      }

      if( type != ZOO_DELETED_EVENT ) // If this is the preivous worker, maybe it is a delete
        if( au::CheckIfStringsBeginWith(path, NODE_WORKER_BASE))
        {
          // Just update information about worker
          size_t worker_id = worker_from_node( path );
          
          au::SharedPointer<samson::gpb::WorkerInfo> worker_info( new samson::gpb::WorkerInfo() );
          int rc = zoo_connection_->Get( path, engine_id() , worker_info.shared_object() );
          if (rc) {
            LM_W(("Error recovering node %s (%s)", path.c_str(), au::zoo::str_error(rc).c_str()));
            return;
          }
          
          AU_M( logs.worker_controller,("Updating from worker %lu node with commit %lu", worker_id , worker_info->last_commit_id() ));
          
          workers_info_.Set(worker_id, worker_info);
          return;
        }
      
      // General Review function to check what´s going on
      int rc = Review();
      if (rc) {
        LM_W(("Error reviewing worker-controller (error %s)", au::zoo::str_error(rc).c_str()));
      }
      return;
    }
    
  }
  
  
  int SamsonWorkerController::GetAllWorkersFromZk() {
    au::TokenTaker tt(&token_);
    
    au::StringVector childrens;
    int rc = zoo_connection_->GetChildrens("/samson/workers", childrens);
    
    if (rc) {
      LM_W(("Error getting workers nodes (%s)", au::zoo::str_error(rc).c_str()));
      return rc;
    }
    
    worker_ids_.clear();
    for (size_t i = 0; i < childrens.size(); ++i) {
      worker_ids_.push_back(atoll(childrens[i].substr(1).c_str())); //  Note children are... /wXXXX
    }
    
    std::sort( worker_ids_.begin() , worker_ids_.end() );

    // Get all information from all workers...
    for (size_t i = 0; i < worker_ids_.size() ; ++i) {
      AU_M( logs.worker_controller, ("Recovering information for worker %lu", worker_ids_[i]));
      
      // Recover information for thi worker
      au::SharedPointer<samson::gpb::WorkerInfo> worker_info( new samson::gpb::WorkerInfo() );

      std::string node = node_for_worker(worker_ids_[i]);
      int rc = zoo_connection_->Get( node, engine_id(), worker_info.shared_object() );
      if (rc) {
        LM_W(("Error recovering node %s (%s)", node.c_str(), au::zoo::str_error(rc).c_str()));
      } else {
        workers_info_.Set( worker_ids_[i], worker_info );
      }
    }
    
    return rc; // OK
  }
  
  std::vector<KVRange> SamsonWorkerController::GetMyKVRanges() {
    au::TokenTaker tt(&token_);
    
    std::vector<KVRange> hg_ranges;
    
    for (int i = 0; i < cluster_info_->process_units_size(); ++i) {
      // Range of this process unit
      int hg_begin = cluster_info_->process_units(i).hg_begin();
      int hg_end = cluster_info_->process_units(i).hg_end();
      
      // Add if I am the main responsible
      if (cluster_info_->process_units(i).worker_id() == worker_id_) {
        hg_ranges.push_back( KVRange(hg_begin, hg_end) );
        continue;
      }
    }
    
    return hg_ranges;
  }
  
  std::vector<KVRange> SamsonWorkerController::GetAllMyKVRanges() {
    au::TokenTaker tt(&token_);
    
    std::vector<KVRange> hg_ranges;
    
    for (int i = 0; i < cluster_info_->process_units_size(); ++i) {
      // Range of this process unit
      int hg_begin = cluster_info_->process_units(i).hg_begin();
      int hg_end = cluster_info_->process_units(i).hg_end();
      
      // Add if I am the main responsible
      if (cluster_info_->process_units(i).worker_id() == worker_id_) {
        hg_ranges.push_back(KVRange(hg_begin, hg_end));
        continue;
      }
      
      // Add if I am a replica responsible
      for (int j = 0; j < cluster_info_->process_units(i).replica_worker_id_size(); ++j) {
        if (cluster_info_->process_units(i).replica_worker_id(j) == worker_id_) {
          hg_ranges.push_back(KVRange(hg_begin, hg_end));
          continue;
        }
      }
    }
    
    return hg_ranges;
  }
  
  
  int SamsonWorkerController::UpdateWorkerNode( size_t last_commit_id ) {

    // Update worker node with information about me
    au::TokenTaker tt(&token_);

    if( worker_info_.last_commit_id() == last_commit_id )
      return 0;

    // Keep previous value, just in case we cannot update in ZK
    size_t previous_last_commit_id = worker_info_.last_commit_id();
    
    // Set the new value of commit id
    worker_info_.set_last_commit_id( last_commit_id );
    
    // Set content of my worker node again
    int rc = zoo_connection_->Set( node_worker_ , &worker_info_);
    
    if (rc) {
      AU_SW(("Not possible to update worker node %s with new commit id %lu"
            , au::zoo::str_error(rc).c_str()
            , last_commit_id ));
      worker_info_.set_last_commit_id( previous_last_commit_id ); // Recover previous value
    }

    AU_M( logs.worker_controller, ("Updating worker node with commit %lu", last_commit_id));
    return rc;
  }

  int SamsonWorkerController::ReviewClusterLeather() {
    
    // I am the cluster leader
    AU_M( logs.worker_controller, ("This worker is the Cluster Leader"));
    
    // Add a watch to all workers ( to be informed if any of them falls down to change cluster setup )
    for (size_t i = 0; i < worker_ids_.size(); ++i) {
      std::string node = au::str("/samson/workers/w%010lu", worker_ids_[i]);
      AU_M( logs.worker_controller, ("Adding watcher on %s", node.c_str()));
      int rc = zoo_connection_->Exists(node, engine_id());
      if (rc) {
        // Recheck since a node has disappeared
        AU_M( logs.worker_controller, ("Error while adding watcher on node %s (%s). Rechecking..."
                               , node.c_str()
                               , au::zoo::str_error(rc).c_str()));
        return 1; // Error, recheck is necessary
      }
    }
    
    // Add an alert on new_worker node to be alerted when a worker is up
    int rc = zoo_connection_->Exists("/samson/new_worker", engine_id() );
    if (rc) {
      LM_W(("Not possible to check in on /samson/new_worker"));
      return 1;
    }
    
    // Make sure node /samson/data is created ( if not, create an empty dat )
    gpb::DataModel data_model;
    rc = zoo_connection_->Get("/samson/data", &data_model);
    if (rc) {
      if (rc != ZNONODE) {
        LM_W(("Error getting folder /samson/data %s", au::zoo::str_error(rc).c_str()));
        return rc;
      }
      
      // Create an empty node
      LM_M((">>> Creating a new data model for this cluster"));
      data_model.set_replication_factor(3);
      
      data_model.set_commit_id(1); // First commit id ever
      gpb::Data * previous_data = data_model.mutable_previous_data();
      previous_data->set_next_stream_operation_id(1);
      previous_data->set_last_commit_id(0);
      gpb::Data * current_data = data_model.mutable_current_data();
      current_data->set_next_stream_operation_id(1);
      current_data->set_last_commit_id(0);
      
      rc = zoo_connection_->Create("/samson/data", 0, &data_model);
      if (rc) {
        LM_W(("Not possible to create initial /samson/data: %s", au::zoo::str_error(rc).c_str()));
        return rc;
      }
      LM_M((">>> New data model created"));
      
    }

    // Review if current cluster information is valid ( new data model if required )
    rc = RecoverClusterInfo();
    
    if( rc && (rc != ZNONODE) )
    {
      LM_W(("Error recovering cluster info at /samson/cluster: %s", au::zoo::str_error(rc).c_str()));
      return 1;
    }
    
    if (rc == ZNONODE) {
      AU_M( logs.worker_controller, ("Creating a new cluster info"));
      int rc2 = CreateClusterInfo(0); // Create a version-0 cluster info
      if (rc2) {
        LM_W(("Not possible to create cluster info %s", au::zoo::str_error(rc2).c_str()));
        return rc2;
      }
      
      AU_M( logs.worker_controller, ("Creating node /samson/cluster"));
      rc = zoo_connection_->Create("/samson/cluster", 0, cluster_info_.shared_object());
      if (rc) {
        LM_W(("Not possible to create cluster node at /samson/cluster (%s)", au::zoo::str_error(rc).c_str()));
        return 1;
      }
      
      // New Cluster info created! We are over.
      engine::notify("notification_new_cluster_setup");
      return 0;
    }
    
    // We have recovered cluster informtion, let see if it is necessary to be changed
    if (!IsValidClusterInfo()) {
      
      // Create a new cluster setup
      int rc2 = CreateClusterInfo( cluster_info_->version() + 1 );
      if (rc2) {
        LM_W(("Not possible to create cluster info %s", au::zoo::str_error(rc2).c_str()));
        return rc2;
      }
      
      // Set the new cluster information to update the other worhers
      rc = zoo_connection_->Set("/samson/cluster", cluster_info_.shared_object());
      if (rc) {
        LM_W(("Not possible to set new version of cluter info: %s", au::zoo::str_error(rc).c_str()));
        return 1;
      }

      // Notify about a new cluster to recover data model
      engine::notify("notification_new_cluster_setup");
    }
    
    // Everything ok
    return 0;
  }

  int SamsonWorkerController::ReviewNonClusterLeather() {
    AU_M( logs.worker_controller, ("I am a Non-Leader worker"));    // I am a normal worker
    
    // Add a watcher to my previous worker to be alerted when it fails down, so I would be promoted to cluster leather
    for (size_t i = 1; i < worker_ids_.size(); ++i) {
      if (worker_ids_[i] == worker_id_) {
        std::string previous_node_worker = au::str("/samson/workers/w%010lu", worker_ids_[i - 1]);
        AU_M( logs.worker_controller, ("Adding watcher on previous worker at %s", previous_node_worker.c_str()));
        int rc = zoo_connection_->Exists(previous_node_worker, engine_id());
        if (rc) {
          AU_M( logs.worker_controller, ("Error while adding watcher on node %s. Rechecking...", previous_node_worker.c_str()));
        } else {
          break; //
        }
      }
    }
    
    return 0;
  }
  
  int SamsonWorkerController::Review() {
    
    AU_M( logs.worker_controller , ("Checking worker controller") );
    
    au::TokenTaker tt(&token_);
    AU_M( logs.worker_controller, ("check..."));
    
    // Get all the workers involved in this cluster and all information
    int rc = GetAllWorkersFromZk();
    if (rc) {
      LM_W(("Not possible check SamsonWorkerController since there was an error getting worker list %s",
            au::zoo::str_error(rc).c_str()));
      cluster_leader_ = false; // For consistency
      return rc; // Error code based on au::zoo::Connection errores
    }
    
    // Check if I am the lowest worker_id ( that means the leader )
    cluster_leader_ = ( worker_ids_[0] == worker_id_ );
    
    if (cluster_leader_) {
      return ReviewClusterLeather();
      
    } else {
      return ReviewNonClusterLeather();
    }
    
  }
  
  bool are_equal(const std::vector<size_t> &a, const std::vector<size_t>& b) {
    if (a.size() != b.size()) {
      return false;
    }
    
    for (size_t i = 0; i < a.size(); ++i) {
      if (a[i] != b[i]) {
        return false;
      }
    }
    return true;
  }
  
  // Review cluster information to see if it is necessary to be updated
  bool SamsonWorkerController::IsValidClusterInfo( ) {
    
    au::TokenTaker tt(&token_);
    
    if (!cluster_leader_) {
      LM_X(1, ("Internal error"));
    }
    if( cluster_info_ == NULL ){
      LM_X(1, ("Internal error"));
    }
    
    // Check all workers in current cluster setup
    std::vector<size_t> worker_ids_in_cluster;
    for ( int w = 0 ; w < cluster_info_->workers_size() ; ++w )
      worker_ids_in_cluster.push_back( cluster_info_->workers(w).worker_id() );
    std::sort( worker_ids_in_cluster.begin()  ,worker_ids_in_cluster.end() );
    
    // If the cluster is unchanged, do not update cluster information
    if (are_equal(worker_ids_, worker_ids_in_cluster )) {
      AU_M( logs.worker_controller, ("Current cluster_info valid with %lu workers", worker_ids_.size()));
      return true;
    }
    
    AU_M( logs.worker_controller, ("ClusterInfo not valid since workers have changed..."));
    return false;
  }
  
  int SamsonWorkerController::CreateClusterInfo( size_t version ) {
    
    cluster_info_.Reset(new gpb::ClusterInfo()); // New cluster info
    cluster_info_->set_version(version);         // Set the version provided
    
    // All information
    AU_M( logs.worker_controller, ("Recovering information for all worker to define cluster"));
    
    // Create a new cluster based on workers information
    AU_M( logs.worker_controller, ("Creating new cluster based on collected information (%lu workers)", workers_info_.size()));
    
    // Add individual worker information
    for ( size_t w = 0 ; w < worker_ids_.size() ; w++ ) {
      
      au::SharedPointer<gpb::WorkerInfo> worker_info =  workers_info_.Get( worker_ids_[w] );
      if( worker_info == NULL )
        continue; // No information about this worker
      
      samson::gpb::ClusterWorker *cluster_worker = cluster_info_->add_workers();
      cluster_worker->set_worker_id(worker_ids_[w]);
      cluster_worker->mutable_worker_info()->CopyFrom( *worker_info.shared_object() );
    }
    
    // Decide how to organize process units
    int replica_factor = 2;
    
    if (worker_ids_.size() == 1) {
      replica_factor = 1;
    } else if (worker_ids_.size() == 2) {
      replica_factor = 2;
    }
    
    // Decide number of process units
    // Number of hash-group divisions
    // @jges: Changing the order of the tests
    int num_units = 8;
    int num_workers = static_cast<int>(worker_ids_.size()); // Number of workers
    if (num_workers > 10) {
      LM_W(("Cluster setup not ready to handle more than 10 workers"));
      //Note: In the future, we have to scale up the 128 limit to handle more workers
    } else if (num_workers > 5) {
      num_units = 128;
    } else if (num_workers > 2) {
      num_units = 64;
    } else if (num_workers > 1) {
      num_units = 16;
    }
    
    int num_units_per_worker = num_units / num_workers; // Number of units per worker
    
    // TODO(@andreu): This should be revised to map ranges to workers coherently based on previous information
    
    for (int i = 0; i < num_units; ++i) {
      KVRange range = GetKVRangeForDivision(i, num_units);
      
      gpb::ProcessUnit *process_unit = cluster_info_->add_process_units();
      process_unit->set_hg_begin(range.hg_begin_);
      process_unit->set_hg_end(range.hg_end_);
      
      // Chose correct worker ( make sure it is a valid worker )
      int w = i / num_units_per_worker;
      while (w >= static_cast<int> (worker_ids_.size())) {
        w -= worker_ids_.size();
      }
      
      // Set main worker
      process_unit->set_worker_id(worker_ids_[w]);
      
      // Set replica
      for (int r = 0; r < (replica_factor - 1); ++r) {
        int ww = w + 1 + r;
        while (ww >= static_cast<int> (worker_ids_.size())) {
          ww -= worker_ids_.size();
        }
        
        process_unit->add_replica_worker_id(worker_ids_[ww]);
      }
    }
    
    // Notify to everybody that cluster_info changed
    engine::Engine::shared()->notify(new engine::Notification("notification_cluster_info_changed_in_worker"));
    
    return 0;   // OK
  }
  
  
  std::set<size_t> SamsonWorkerController::GetWorkerIds() {
    std::set<size_t> worker_ids;
    for (int i = 0; i < cluster_info_->workers_size(); ++i) {
      worker_ids.insert(cluster_info_->workers(i).worker_id());
    }
    return worker_ids;
  }
  
  au::Uint64Set SamsonWorkerController::GetAllWorkerIdsForRange(KVRange range) {
    // Set of identifier to return
    au::Uint64Set worker_ids;
    
    for (int i = 0; i < cluster_info_->process_units_size(); ++i) {
      const gpb::ProcessUnit& process_unit = cluster_info_->process_units(i);
      
      // Get range for this process unit
      KVRange process_unit_range(process_unit.hg_begin(), process_unit.hg_end());
      
      if (process_unit_range.IsOverlapped(range)) {
        worker_ids.insert(process_unit.worker_id()); // Add replicas
        for (int r = 0; r < process_unit.replica_worker_id_size(); ++r)
          worker_ids.insert(process_unit.replica_worker_id(r));
      }
    }
    return worker_ids;
  }
  
  // Get workers that should have a copy of a block in this range
  au::Uint64Set SamsonWorkerController::GetWorkerIdsForRange(KVRange range) {
    // Set of identifier to return
    au::Uint64Set worker_ids;
    
    for (int i = 0; i < cluster_info_->process_units_size(); ++i) {
      const gpb::ProcessUnit& process_unit = cluster_info_->process_units(i);
      
      // Get range for this process unit
      KVRange process_unit_range(process_unit.hg_begin(), process_unit.hg_end());
      
      if (process_unit_range.IsOverlapped(range)) {
        // Add the main worker id
        size_t tmp_worker_id = process_unit.worker_id();
        if (tmp_worker_id != worker_id_) {
          worker_ids.insert(tmp_worker_id); // Add replicas
        }
        for (int r = 0; r < process_unit.replica_worker_id_size(); ++r) {
          size_t tmp_worker_id = process_unit.replica_worker_id(r);
          if (tmp_worker_id != worker_id_) {
            worker_ids.insert(tmp_worker_id);
          }
        }
      }
    }
    
    return worker_ids;
  }
  
  std::string SamsonWorkerController::get_local_ip() {
    struct ifaddrs *addrs;
    struct ifaddrs *iap;
    struct sockaddr_in *sa;
    char buf[64];
    
    getifaddrs(&addrs);
    for (iap = addrs; iap != NULL; iap = iap->ifa_next) {
      if (iap->ifa_addr && (iap->ifa_flags & IFF_UP) && iap->ifa_addr->sa_family == AF_INET) {
        sa = (struct sockaddr_in *) (iap->ifa_addr);
        inet_ntop(iap->ifa_addr->sa_family, reinterpret_cast<void *>(&(sa->sin_addr)), buf, sizeof(buf));
        
        if (strcmp(buf, "127.0.0.1") != 0) {
          freeifaddrs(addrs);
          return buf;
        }
      }
    }
    
    freeifaddrs(addrs);
    AU_W( logs.worker_controller, ("Using 127.0.0.1 as local ip since no other local ip found"));
    return "127.0.0.1";
  }
  
  size_t SamsonWorkerController::get_new_block_id() {
    
    
    if( worker_id_ == (size_t) -1 )
      return (size_t) -1;
    
    // [worker 32bits][Counter 32bits]
    BlockId block_id;

    block_id.uint32[0] = (unsigned int) worker_id_;
    block_id.uint32[1] = block_id_counter_++;;
    
    return block_id.uint64;
    
  }
  
  size_t worker_from_block_id( size_t block_id )
  {
    BlockId b;
    b.uint64 = block_id;
    return b.uint32[0];
  }
  
  std::set<size_t> SamsonWorkerController::GetWorkers()
  {
    std::set<size_t> workers;
    au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = cluster_info_;
    for ( int w = 0 ; w < cluster_info->workers_size() ; w++)
      workers.insert(cluster_info->workers(w).worker_id());
    return workers;
    
  }
  
  size_t SamsonWorkerController::GetMyLastCommitId()
  {
    return worker_info_.last_commit_id();
  }
  
  bool SamsonWorkerController::CheckDataModelCommitId( size_t last_commit_id )
  {
    if( worker_ids_.size() == 0 )
    {
      return false; // Check for secutrity
    }
    
    for ( size_t w = 0 ; w < worker_ids_.size()  ; ++w )
    {
      au::SharedPointer<gpb::WorkerInfo> info = workers_info_.Get( worker_ids_[w] );
      if( info == NULL )
      {
        return false; // no information, no commitment
      }
      
      if( info->last_commit_id() < last_commit_id )
      {
        //AU_SW(("Checking data model %lu: Worker %lu is still at commit id %lu", last_commit_id, worker_ids_[w] , info->last_commit_id() ) );
        return false;
      }
    }
    return true;
    
  }
  
  
}
