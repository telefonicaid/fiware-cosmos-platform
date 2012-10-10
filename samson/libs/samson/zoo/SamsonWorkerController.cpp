#include "samson/zoo/SamsonWorkerController.h"

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

#include "samson/common/common.h"
#include "samson/common/KVRange.h"
#include "samson/stream/BlockManager.h"
#include "samson/zoo/common.h"

namespace samson {
SamsonWorkerController::SamsonWorkerController(zoo::Connection *zoo_connection, int port, int port_web) :
  token_("SamsonWorkerController") {
  // zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);

  // Keep a pointer to the connection
  zoo_connection_ = zoo_connection;

  // Make sure basic folders are created
  LM_T(LmtClusterSetup, ("Creating basic folders in zk. Jsut in case I am the first one..."));
  zoo_connection_->Create("/samson");
  zoo_connection_->Create("/samson/workers");

  // Keep information about this worker
  port_ = port;
  port_web_ = port_web;

  // Fill basic information for this worker
  worker_info_.set_host(get_local_ip());
  worker_info_.set_port(port_);
  worker_info_.set_port_web(port_web_);
  worker_info_.set_cores(au::Singleton<SamsonSetup>::shared()->getUInt64("general.num_processess"));
  worker_info_.set_memory(au::Singleton<SamsonSetup>::shared()->getUInt64("general.memory"));
  worker_info_.set_ready(false);   // By default, we are not ready now...

  // Notification every 5 seconds to recover cluster information anywat
  listen("notification_SamsonWorkerController_review");
  engine::Engine::shared()->notify(new engine::Notification("notification_SamsonWorkerController_review"), 5);
}

int SamsonWorkerController::init() {
  LM_T(LmtClusterSetup, ("SamsonWorkerController constructor"));

  // Create ephemeral node with my information
  node_worker_ = NODE_WORKER_BASE;
  int rc = zoo_connection_->Create(node_worker_, ZOO_SEQUENCE | ZOO_EPHEMERAL, &worker_info_);
  if (rc) {
    LM_W(("Not possible to create ephemeral worker node at %s (%s)"
            , node_worker_.c_str()
            , zoo::str_error(rc).c_str()));
    return rc;
  }

  LM_T(LmtClusterSetup, ("Ephemeral node created at %s", node_worker_.c_str()));

  // Get my assigned worker_id
  worker_id_ = atoll(node_worker_.substr(strlen(NODE_WORKER_BASE)).c_str());
  LM_T(LmtClusterSetup, ("Assigned Worker id: %lu", worker_id_));

  // Repeat check until it returns 0
  rc = check();
  if (rc) {
    LM_W(("Not possible to check samson worker controller %s (%s)"
            , node_worker_.c_str()
            , zoo::str_error(rc).c_str()));
    return rc;
  }

  if (!cluster_leader_) {
    rc = zoo_connection_->Set("/samson/new_worker", "0", 1);
    if (rc) {
      LM_W(("Error touching node /samson/new/worker (%s)", samson::zoo::str_error(rc).c_str()));
      return rc;
    }
  }

  // Recover cluster information ( independently if I am or not the leader of the cluster )
  rc = recover_cluster_info();
  if (rc) {
    LM_W(("Not possible to recover cluster information %s (%s)"
            , node_worker_.c_str()
            , zoo::str_error(rc).c_str()));
    return rc;
  }

  return rc; // OK
}

int SamsonWorkerController::recover_cluster_info() {
  au::TokenTaker tt(&token_);

  // Recover cluster information and add watch for updates
  LM_T(LmtClusterSetup, ("Recovering cluster information"));

  au::SharedPointer<gpb::ClusterInfo> cluster_info(new gpb::ClusterInfo());

  Stat stat;
  int rc = zoo_connection_->Get("/samson/cluster", engine_id(), cluster_info.shared_object(), &stat);
  if (rc) {
    LM_W(("Not possible to recover cluster information (%s)", samson::zoo::str_error(rc).c_str()));
    return rc;
  }

  if ((cluster_info_ == NULL) || (cluster_info->version() > cluster_info_->version())) {
    // New version, so replace current version of the cluster and alert the system
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
    LM_T(LmtClusterSetup, ("SamsonWorkerController watch %s", path.c_str()));
    if (path != "/samson/cluster") {
      return; // Only this path is considered
    }
    int rc = recover_cluster_info(); // Rest of watchers are related with workers up or down
    if (rc) {
      LM_W(("Error recovering cluster information (error %s)", zoo::str_error(rc).c_str()));
    }
    rc = check();
    if (rc) {
      LM_W(("Error reviewing worker-controller (error %s)", zoo::str_error(rc).c_str()));
    }
    return;
  }

  if (notification->isName("notification_SamsonWorkerController_review")) {
    int rc = recover_cluster_info(); // Rest of watchers are related with workers up or down
    if (rc) {
      LM_W(("Error recovering cluster information (error %s)", zoo::str_error(rc).c_str()));
    }
    rc = check();
    if (rc) {
      LM_W(("Error reviewing worker-controller (error %s)", zoo::str_error(rc).c_str()));
    }
    return;
  }
}

std::vector<size_t> SamsonWorkerController::get_all_workers_from_cluster_info(
                                                                              au::SharedPointer<
                                                                                  samson::gpb::ClusterInfo> cluster_info) {
  au::TokenTaker tt(&token_);

  // Get current worker ids included in the current cluster_model
  std::vector<size_t> worker_ids;

  for (int i = 0; i < cluster_info->workers_size(); i++) {
    worker_ids.push_back(cluster_info->workers(i).worker_id());
  }

  // Sort ids
  std::sort(worker_ids.begin(), worker_ids.end());

  return worker_ids;
}

int SamsonWorkerController::GetBlockMap(std::multimap<size_t, size_t>& blocks_map) {
  // Recover all workers
  std::vector<size_t> worker_ids;
  int rc = get_all_workers_from_zk(worker_ids);

  // error getting this list
  if (rc) {
    return rc;
  }

  for (size_t i = 0; i < worker_ids.size(); i++) {
    size_t worker_id = worker_ids[i];
    std::string path = node_for_worker(worker_id);
    gpb::WorkerInfo worker_info;
    int rc = zoo_connection_->Get(path, &worker_info);

    if (rc) {
      LM_W(("Not possible to recover information for worker %lu. Ignored", worker_id));
    } else {
      for (int i = 0; i < worker_info.block_id_size(); i++) {
        size_t block_id = worker_info.block_id(i);
        blocks_map.insert(std::pair<size_t, size_t>(block_id, worker_id));
      }
    }
  }

  return rc;
}

int SamsonWorkerController::get_all_workers_from_zk(std::vector<size_t>& worker_ids) {
  au::TokenTaker tt(&token_);

  au::StringVector childrens;
  int rc = zoo_connection_->GetChildrens("/samson/workers", childrens);

  if (rc) {
    LM_W(("Error getting workers nodes (%s)", zoo::str_error(rc).c_str()));
    return rc;
  }

  for (size_t i = 0; i < childrens.size(); i++) {
    worker_ids.push_back(atoll(childrens[i].substr(1).c_str())); //  Note children are... /wXXXX
  }
  // Sort ids
  std::sort(worker_ids.begin(), worker_ids.end());

  return rc; // OK
}

KVRanges SamsonWorkerController::GetMyKVRanges() {
  au::TokenTaker tt(&token_);

  KVRanges hg_ranges;

  for (int i = 0; i < cluster_info_->process_units_size(); i++) {
    // Range of this process unit
    int hg_begin = cluster_info_->process_units(i).hg_begin();
    int hg_end = cluster_info_->process_units(i).hg_end();

    // Add if I am the main responsible
    if (cluster_info_->process_units(i).worker_id() == worker_id_) {
      hg_ranges.Add(KVRange(hg_begin, hg_end));
      continue;
    }
  }

  return hg_ranges;
}

KVRanges SamsonWorkerController::GetAllMyKVRanges() {
  au::TokenTaker tt(&token_);

  KVRanges hg_ranges;

  for (int i = 0; i < cluster_info_->process_units_size(); i++) {
    // Range of this process unit
    int hg_begin = cluster_info_->process_units(i).hg_begin();
    int hg_end = cluster_info_->process_units(i).hg_end();

    // Add if I am the main responsible
    if (cluster_info_->process_units(i).worker_id() == worker_id_) {
      hg_ranges.Add(KVRange(hg_begin, hg_end));
      continue;
    }

    // Add if I am a replica responsible
    for (int j = 0; j < cluster_info_->process_units(i).replica_worker_id_size(); j++) {
      if (cluster_info_->process_units(i).replica_worker_id(j) == worker_id_) {
        hg_ranges.Add(KVRange(hg_begin, hg_end));
        continue;
      }
    }
  }

  return hg_ranges;
}

bool SamsonWorkerController::IsClusterReady() {
  // Recover all workers
  std::vector<size_t> worker_ids;
  if (get_all_workers_from_zk(worker_ids)) {
    return false;
  }

  for (size_t i = 0; i < worker_ids.size(); i++) {
    size_t worker_id = worker_ids[i];
    std::string path = node_for_worker(worker_id);
    gpb::WorkerInfo worker_info;
    int rc = zoo_connection_->Get(path, &worker_info);

    if (rc) {
      LM_W(("Not possible to recover information for worker %lu. Ignored", worker_id));
      return false;
    } else {
      if (!worker_info.ready()) {
        return false;
      }
    }
  }
  return true;
}

int SamsonWorkerController::UpdateWorkerNode(bool worker_ready) {
  // Update worker node with information about me
  au::TokenTaker tt(&token_);

  if (cronomter_update_worker_node_.seconds() < 2) {
    return 0;
  }

  cronomter_update_worker_node_.Reset();

  // Compiler doesn't like the "<::" sequence
  ::google::protobuf::RepeatedField< ::google::protobuf::uint64 > *gpb_block_ids = worker_info_.mutable_block_id();
  worker_info_.set_ready(worker_ready);

  // Get identifiers of blocks in local block manager
  std::set<size_t> block_ids = stream::BlockManager::shared()->GetBlockIds();

  // Insert in the gpb message
  gpb_block_ids->Clear();
  for (std::set<size_t>::iterator b = block_ids.begin(); b != block_ids.end(); b++) {
    gpb_block_ids->Add(*b);
  }

  // Set content of my worker node again
  int rc = zoo_connection_->Set(node_worker_, &worker_info_);
  if (rc) {
    LM_W(("Not possible to update worker node %s", zoo::str_error(rc).c_str()));
  }
  return rc;
}

int SamsonWorkerController::check() {
  au::TokenTaker tt(&token_);

  LM_T(LmtClusterSetup, ("check..."));

  // Get all the workers involved in this cluster
  std::vector<size_t> worker_ids;
  int rc = get_all_workers_from_zk(worker_ids);

  if (rc) {
    LM_W(("Not possible check SamsonWorkerController since there was an error getting worker list %s",
            zoo::str_error(rc).c_str()));
    cluster_leader_ = false; // For consistency
    return rc; // TODO(@andreu): Define this error
  }

  // Check if I am the lowest worker_id ( that means the leader )
  cluster_leader_ = (worker_ids[0] == worker_id_);

  if (cluster_leader_) {
    // I am the cluster leader
    LM_T(LmtClusterSetup, ("I am the Cluster Leader"));

    // Add a watch to all workers ( to be informed if any of them falls down... )
    for (size_t i = 0; i < worker_ids.size(); i++) {
      std::string node = au::str("/samson/workers/w%010lu", worker_ids[i]);
      LM_T(LmtClusterSetup, ("Adding watcher on %s", node.c_str()));
      int rc = zoo_connection_->Exists(node, engine_id());
      if (rc) {
        // Recheck since a node has disapeared
        LM_T(LmtClusterSetup, ("Error while adding watcher on node %s (%s). Rechecking..."
                , node.c_str()
                , samson::zoo::str_error(rc).c_str()));
        return 1; // Error, recheck is necessary
      }
    }

    // Create a node to be notified when a new worker is added
    int rc = zoo_connection_->Create("/samson/new_worker", 0, "0", 1);
    if (rc && (rc != ZNODEEXISTS)) {
      LM_W(("Not possible to create node /samson/new_worker to alert when workers are connected"));
      return 1;
    }
    // Add a watcher over this node ( TODO(@andreu): Need to check this node has not been modified )
    rc = zoo_connection_->Exists("/samson/new_worker", engine_id());
    if (rc) {
      LM_W(("Not possible to check in on /samson/new_worker"));
      return 1;
    }

    // Make sure node /samson/data is created if it does not exist
    gpb::Data data;
    data.set_next_stream_operation_id(1);
    rc = zoo_connection_->Get("/samson/data", &data);
    if (rc) {
      if (rc != ZNONODE) {
        LM_W(("Error getting folder /samson/data %s", samson::zoo::str_error(rc).c_str()));
        return rc;
      }

      // Create an empty node
      rc = zoo_connection_->Create("/samson/data", 0, &data);
      if (rc) {
        LM_W(("Not possible to create empty node /samson/data: %s", samson::zoo::str_error(rc).c_str()));
        return rc;
      }
    }

    // Make sure node /samson/blocks is created if it does not exist
    rc = zoo_connection_->Exists("/samson/blocks");
    if (rc) {
      if (rc != ZNONODE) {
        LM_W(("Error getting folder /samson/blocks %s", samson::zoo::str_error(rc).c_str()));
        return rc;
      }

      // Create an empty node
      rc = zoo_connection_->Create("/samson/blocks", 0);
      if (rc) {
        LM_W(("Not possible to create empty node /samson/blocks: %s", samson::zoo::str_error(rc).c_str()));
        return rc;
      }
    }

    // Make sure cluster_info is setup correctly
    return check_cluster_info();
  } else {
    // I am a normal worker
    LM_T(LmtClusterSetup, ("I am a Non-Leader worker"));

    // Add a watcher to my previous worker to be alerted when it fails down
    for (size_t i = 1; i < worker_ids.size(); i++) {
      if (worker_ids[i] == worker_id_) {
        std::string node = au::str("/samson/workers/w%010lu", worker_ids[i - 1]);
        LM_T(LmtClusterSetup, ("Adding watcher on previous worker at %s", node.c_str()));
        int rc = zoo_connection_->Exists(node, engine_id());
        if (rc) {
          // Recheck since a node has disapeared
          LM_T(LmtClusterSetup, ("Error while adding watcher on node %s. Rechecking...", node.c_str()));
          return 1;
        } else {
          break; //
        }
      }

      // Check if I have not found myself in the list
      if (i == (worker_ids.size() - 1)) {
        LM_W(("I cannot found myself in the list of workers"));
        return 1;
      }
    }
  }

  // Everything ok
  return 0;
}

bool are_equal(const std::vector<size_t> &a, const std::vector<size_t>& b) {
  if (a.size() != b.size()) {
    return false;
  }

  for (size_t i = 0; i < a.size(); i++) {
    if (a[i] != b[i]) {
      return false;
    }
  }

  return true;
}

// Review cluster information to see if it is necessary to be updated
bool SamsonWorkerController::is_valid_cluster_info(au::SharedPointer<samson::gpb::ClusterInfo> cluster_info) {
  au::TokenTaker tt(&token_);

  if (!cluster_leader_) {
    LM_X(1, ("Internal error"));
  }LM_T(LmtClusterSetup, ("Updating cluster info"));

  // Get all workers
  std::vector<size_t> worker_ids;
  int rc = get_all_workers_from_zk(worker_ids);
  if (rc) {
    LM_W(("Not possible to validate cluster information. %s", zoo::str_error(rc).c_str()));
    return false; // We cannot check
  }

  // Get all workers from current cluster information
  std::vector<size_t> current_worker_ids = get_all_workers_from_cluster_info(cluster_info);

  // If the cluster is unchanged, do not update cluster information
  if (are_equal(worker_ids, current_worker_ids)) {
    LM_T(LmtClusterSetup, ("Current cluster_info valid with %lu workers", worker_ids.size()));
    return true;
  }

  LM_T(LmtClusterSetup, ("ClusterInfo not valid since workers have changed..."));
  return false;
}

int SamsonWorkerController::create_cluster_info(size_t version) {
  // New cluster info
  cluster_info_.Reset(new gpb::ClusterInfo());
  cluster_info_->set_version(version);

  // All information
  LM_T(LmtClusterSetup, ("Recovering information for all worker to define cluster"));

  std::vector<size_t> worker_ids;
  int rc = get_all_workers_from_zk(worker_ids);

  if (rc) {
    LM_W(("Not possible to create cluster info since we cannot recover list of workers %s", zoo::str_error(rc).c_str()));
    return rc;
  }

  au::map<size_t, samson::gpb::WorkerInfo> workers_info;
  for (int i = 0; i < static_cast<int>(worker_ids.size()); i++) {
    LM_T(LmtClusterSetup, ("Recovering information for worker %lu", worker_ids[i]));

    // Recover information for thi worker
    samson::gpb::WorkerInfo *worker_info = new samson::gpb::WorkerInfo();

    std::string node = node_for_worker(worker_ids[i]);
    int rc = zoo_connection_->Get(node, worker_info);
    if (rc) {
      LM_W(("Error recovering node %s (%s)", node.c_str(), samson::zoo::str_error(rc).c_str()));
      LM_W(("Ignoring worker %lu in cluster setup."));
      delete worker_info;
    } else {
      workers_info.insertInMap(worker_ids[i], worker_info);
    }
  }

  // Create a new cluster based on workers information
  LM_T(LmtClusterSetup, ("Creating new cluster based on collected information (%lu workers)", workers_info.size()));

  au::map<size_t, samson::gpb::WorkerInfo>::iterator it;
  for (it = workers_info.begin(); it != workers_info.end(); it++) {
    samson::gpb::ClusterWorker *cluster_worker = cluster_info_->add_workers();
    cluster_worker->set_worker_id(it->first);
    cluster_worker->mutable_worker_info()->CopyFrom(*it->second);
  }

  // Decide how to organize process units
  int replica_factor = 3;

  if (worker_ids.size() == 1) {
    replica_factor = 1;
  } else if (worker_ids.size() == 2) {
    replica_factor = 2; // Decide number of process units
  }
  int num_units = 16 * worker_ids.size(); // Number of divisions

  // Number of workers
  int num_workers = worker_ids.size();

  // Number of units per worker
  int num_units_per_worker = num_units / num_workers;

  for (int i = 0; i < num_units; i++) {
    KVRange range = rangeForDivision(i, num_units);

    gpb::ProcessUnit *process_unit = cluster_info_->add_process_units();
    process_unit->set_hg_begin(range.hg_begin);
    process_unit->set_hg_end(range.hg_end);

    // Chose correct worker ( make sure it is a valid worker )
    int w = i / num_units_per_worker;
    while (w >= static_cast<int>(worker_ids.size())) {
      w -= worker_ids.size();
    }

    // Set main worker
    process_unit->set_worker_id(worker_ids[w]);

    // Set replica
    for (int r = 0; r < (replica_factor - 1); r++) {
      int ww = w + 1 + r;
      while (ww >= static_cast<int>(worker_ids.size())) {
        ww -= worker_ids.size();
      }

      process_unit->add_replica_worker_id(worker_ids[ww]);
    }
  }

  // Remove collected information from workers
  workers_info.clearMap();

  // Notify to everybody that cluster_info changed
  engine::Engine::shared()->notify(new engine::Notification("notification_cluster_info_changed_in_worker"));

  return 0; // OK
}

int SamsonWorkerController::check_cluster_info() {
  // This process is only done by leader
  if (!cluster_leader_) {
    LM_X(1, ("Internal error"));
  }

  LM_T(LmtClusterSetup, ("Checking cluster info"));

  // Recover current information for the cluster
  LM_T(LmtClusterSetup, ("Recovering current cluster info"));

  au::SharedPointer<gpb::ClusterInfo> cluster_info(new gpb::ClusterInfo()); // Temporal cluster info
  int rc = zoo_connection_->Get("/samson/cluster", cluster_info.shared_object());

  if (rc == ZNONODE) {
    LM_T(LmtClusterSetup, ("Creating a new cluster info"));
    int rc2 = create_cluster_info(0); // Create a version-0 cluster info

    if (rc2) {
      LM_W(("Not possible to create cluster info %s", zoo::str_error(rc2).c_str()));
      return rc2;
    }

    LM_T(LmtClusterSetup, ("Creating node /samson/cluster"));
    rc = zoo_connection_->Create("/samson/cluster", 0, cluster_info_.shared_object());
    if (rc) {
      LM_W(("Not possible to create cluster node at /samson/cluster (%s)"
              , samson::zoo::str_error(rc).c_str()));
      return 1;
    }

    // Cluster node created!
    return 0;
  } else if (rc) {
    // Any other error
    LM_W(("Error recovering cluster info at /samson/cluster: %s", samson::zoo::str_error(rc).c_str()));
    return 1;
  }

  // We have recovered cluster informtion, let see if it is necessary to be changed
  if (!is_valid_cluster_info(cluster_info)) {
    // Create a new cluster setup
    int rc2 = create_cluster_info(cluster_info->version() + 1);
    if (rc2) {
      LM_W(("Not possible to create cluster info %s", zoo::str_error(rc2).c_str()));
      return rc2;
    }

    // Set the new cluster information to update the other worhers
    rc = zoo_connection_->Set("/samson/cluster", cluster_info_.shared_object());
    if (rc) {
      LM_W(("Not possible to set new version of cluter info: %s", samson::zoo::str_error(rc).c_str()));
      return 1;
    }
  }

  // Everything ok
  return 0;
}

au::SharedPointer<samson::gpb::ClusterInfo> SamsonWorkerController::GetCurrentClusterInfo() {
  return cluster_info_;
}

size_t SamsonWorkerController::worker_id() {
  return worker_id_;
}

std::set<size_t> SamsonWorkerController::GetWorkerIds() {
  std::set<size_t> worker_ids;
  for (int i = 0; i < cluster_info_->workers_size(); i++) {
    worker_ids.insert(cluster_info_->workers(i).worker_id());
  }
  return worker_ids;
}

au::Uint64Set SamsonWorkerController::GetAllWorkerIdsForRange(KVRange range) {
  // Set of identifier to return
  au::Uint64Set worker_ids;

  for (int i = 0; i < cluster_info_->process_units_size(); i++) {
    const gpb::ProcessUnit& process_unit = cluster_info_->process_units(i);

    // Get range for this process unit
    KVRange process_unit_range(process_unit.hg_begin(), process_unit.hg_end());

    if (process_unit_range.IsOverlapped(range)) {
      worker_ids.insert(process_unit.worker_id()); // Add replicas
      for (int r = 0; r < process_unit.replica_worker_id_size(); r++)
        worker_ids.insert(process_unit.replica_worker_id(r));
    }
  }
  return worker_ids;
}

// Get workers that should have a copy of a block in this range
au::Uint64Set SamsonWorkerController::GetWorkerIdsForRange(KVRange range) {
  // Set of identifier to return
  au::Uint64Set worker_ids;

  for (int i = 0; i < cluster_info_->process_units_size(); i++) {
    const gpb::ProcessUnit& process_unit = cluster_info_->process_units(i);

    // Get range for this process unit
    KVRange process_unit_range(process_unit.hg_begin(), process_unit.hg_end());

    if (process_unit_range.IsOverlapped(range)) {
      // Add the main worker id
      size_t tmp_worker_id = process_unit.worker_id();
      if (tmp_worker_id != worker_id_) {
        worker_ids.insert(tmp_worker_id); // Add replicas
      }
      for (int r = 0; r < process_unit.replica_worker_id_size(); r++) {
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
  LM_W(("Using 127.0.0.1 as local ip since no other local ip found"));
  return "127.0.0.1";
}

size_t SamsonWorkerController::get_new_block_id() {
  std::string path_base = "/samson/blocks/b";
  std::string path = path_base;

  int rc = zoo_connection_->Create(path, ZOO_SEQUENCE | ZOO_EPHEMERAL);

  if (rc) {
    // Some error...
    LM_W(("Error (%d) creating node to get a new block id: %s", rc, samson::zoo::str_error(rc).c_str()));
    return (size_t) -1;
  } else {
    // Created blocks
    size_t block_id = atoll(path.substr(path_base.length()).c_str());

    // Remove node
    rc = zoo_connection_->Remove(path);
    if (rc) {
      LM_W(("Error (%d), not possible to remove node at %s", rc, path.c_str()));
    }
    // Check non zero id generated
    // TODO(@andreu): please check if 0 ids are really wrong
    // Why? Apparently, zookeeper is giving block ids starting from 0
    if (block_id == 0) {
      LM_W(("Wrong block_id generated from path '%s'", path.c_str()));
      //      return (size_t)-1;
    }

    return block_id;
  }
}
}
