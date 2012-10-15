#ifndef _H_SAMSON_ZOO_NODE_COMITTER
#define _H_SAMSON_ZOO_NODE_COMITTER

#include <list>
#include <string>
#include <vector>

#include "au/containers/SharedPointer.h"
#include "au/containers/Uint64Vector.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/NotificationListener.h"

#include "samson/common/gpb_operations.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/samson.pb.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/zoo/CommitCommand.h"
#include "samson/zoo/common.h"
#include "samson/zoo/Connection.h"

#define NODE_WORKER_BASE "/samson/workers/w"

// ------------------------------------------------------------------
//
// class ZooNodeCommiter
//
// Class that allows synch updates over a node in zookeeper
// using a "std::string" as commit info and google protocol buffers
// as serialization in the zk node
//
// ------------------------------------------------------------------

namespace samson {
struct CommitRecord {
    std::string caller;
    std::string commit_command;
    std::string result;
    double commit_time;

    CommitRecord(const std::string& _caller
                 , const std::string& _commit_command
                 , const std::string& _result
                 , double _commit_time) {
      caller = _caller;
      commit_command = _commit_command;
      result = _result;
      commit_time = _commit_time;
    }
};

template<class C>   // C is suppoused to be a gpb message class
class ZooNodeCommiter : public engine::NotificationListener {
  public:
    // Constructor & destructors
    ZooNodeCommiter(zoo::Connection *zoo_connection, const std::string& path) :
      token_("ZooNodeCommiter") {
      // Keep a pointer to the connection
      zoo_connection_ = zoo_connection;

      // Path in the zk tree
      path_ = path;

      // Get data from zk
      GetDataFromZooNode();
    }

    virtual ~ZooNodeCommiter() {
    }

    // Virtual function to update internal state with a provided commit
    virtual void
    PerformCommit(au::SharedPointer<C> c, std::string commit_command, int version, au::ErrorManager *error) = 0;

    // Virtual method to be nofitied when and update comes up
    virtual void NotificationNewModel(int version, au::SharedPointer<C> c) {
    }

    // Get a duplicated version of the model ( node that C is a GPB message )
    au::SharedPointer<C> getCurrentModel() {
      au::TokenTaker tt(&token_);
      return c_;
    }

    // virtual method of engine::NotificationListener
    virtual void notify(engine::Notification *notification) {
      au::TokenTaker tt(&token_);   // Mutex protection
      GetDataFromZooNode();   // Recover data from zk
    }

    // Commit a new commit_command
    void Commit(const std::string& caller, const std::string& commit_command, au::ErrorManager *error) {

      // Perform real commit
      au::Cronometer cronometer;
      InternCommit(commit_command, error);
      double time = cronometer.seconds();
      
      // Log activity for debugging
      if (error->IsActivated()) {
        last_commits_.push_front(CommitRecord(caller, commit_command, error->GetMessage() , time));
      } else {
        last_commits_.push_front(CommitRecord(caller, commit_command, "OK" , time));
      }

      while (last_commits_.size() > 100) {
        last_commits_.pop_back();
      }
    }

    au::SharedPointer<gpb::Collection> GetLastCommitsCollection(const Visualization& visualization) {
      au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
      collection->set_name("last_commits");

      int num = 0;
      std::list<CommitRecord>::iterator it;
      for (it = last_commits_.begin(); it != last_commits_.end(); it++) {
        {
          gpb::CollectionRecord *record = collection->add_record();
          ::samson::add(record, "#commit", num, "different");
          ::samson::add(record, "Concept", "Caller", "different");
          ::samson::add(record, "Value", it->caller, "different");
        }

        std::vector<std::string> components = au::split(it->commit_command, ' ');
        for (size_t i = 0; i < components.size(); i++) {
          gpb::CollectionRecord *record = collection->add_record();
          ::samson::add(record, "#commit", num, "different");
          ::samson::add(record, "Concept", "Command", "different");
          ::samson::add(record, "Value", components[i], "different");
        }

        {
          gpb::CollectionRecord *record = collection->add_record();
          ::samson::add(record, "#commit", num, "different");
          ::samson::add(record, "Concept", "Result", "different");
          ::samson::add(record, "Value", it->result, "different");
        }
        
        {
          gpb::CollectionRecord *record = collection->add_record();
          ::samson::add(record, "#commit", num, "different");
          ::samson::add(record, "Concept", "Time", "different");
          ::samson::add(record, "Value", au::str(it->commit_time), "different");
        }
        
        num++;
      }

      return collection;
    }

  private:
    int GetDataFromZooNode() {
      // New candidate for data
      au::SharedPointer<C> c(new C());

      int rc = zoo_connection_->WaitUntilConnected(5000);
      if (rc) {
        return rc;
      }

      int previous_version = stat_.version;
      {
        au::TokenTaker tt(&token_);   // Mutex protection
        rc = zoo_connection_->Get(path_.c_str(), engine_id(), c.shared_object(), &stat_);
      }

      if (rc) {
        LM_W(("Not possible to get node %s from zk: %s"
                , path_.c_str()
                , samson::zoo::str_error(rc).c_str()));
        return rc;
      }

      // Keep this new data model
      c_ = c;

      // Check if we have a new version now...
      if (stat_.version > previous_version) {
        NotificationNewModel(stat_.version, c_);
      }

      return 0;   // Everything ok
    }

    void InternCommit(std::string commit_command, au::ErrorManager *error) {
      // Mutex protection
      au::TokenTaker tt(&token_);

      int trial = 0;

      while (true) {
        trial++;
        int rc = GetDataFromZooNode();   // Get data from zk
        if (rc) {
          error->set(au::str("Error with ZK: %s", zoo::str_error(rc).c_str()));
          return;
        }

        PerformCommit(c_, commit_command, stat_.version, error);   // Real changes on data model

        if (error->IsActivated()) {
          GetDataFromZooNode();   // Model is recovered from zk to make sure it is not affected
          return;   // No commit is done
        }

        // Try to commit
        rc = zoo_connection_->Set(path_.c_str(), c_.shared_object(), stat_.version);

        if (rc == ZBADVERSION) {
          // Wrong version ( this means another commit was accepted first )
          LM_W(("Wrong version of zk path %s (%lu)", path_.c_str(), stat_.version));
          continue;   // Loop to load again data model and commit
        }

        if (rc) {
          error->set(au::str("Error with ZK: %s ", zoo::str_error(rc).c_str()));
          return;
        }

        // Operation is commited correctly
        break;
      }

      // Recover data from zk to get the last version
      GetDataFromZooNode();
    }

    Stat stat_;   // zk state for data node
    au::SharedPointer<C> c_;   // Current data
    std::string path_;   // Path to commit in the zk
    zoo::Connection *zoo_connection_;   // zk conneciton to keep sync
    au::Token token_;   // Mutex protection

    std::list<CommitRecord> last_commits_;
};
}

#endif  // ifndef _H_SAMSON_ZOO_NODE_COMITTER
