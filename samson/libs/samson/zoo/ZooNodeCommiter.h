
#ifndef _H_SAMSON_ZOO_NODE_COMITTER
#define _H_SAMSON_ZOO_NODE_COMITTER

#include "au/containers/SharedPointer.h"
#include "au/containers/Uint64Vector.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/Object.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/gpb_operations.h"
#include "samson/common/samson.pb.h"
#include "samson/zoo/CommitCommand.h"
#include "samson/zoo/Connection.h"
#include "samson/zoo/ConnectionWatcherInterface.h"
#include "samson/zoo/common.h"

#include "samson/common/MessagesOperations.h"
#include "samson/common/Visualitzation.h"

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

  CommitRecord(const std::string& _caller, const std::string& _commit_command, const std::string& _result) {
    caller = _caller;
    commit_command = _commit_command;
    result = _result;
  }
};

template<class C>   // C is suppoused to be a gpb message class
class ZooNodeCommiter : public samson::zoo::ConnectionWatcherInterface {
public:

  // Constructor & destructors
  ZooNodeCommiter(zoo::Connection *zoo_connection, const std::string& path) : token_("ZooNodeCommiter") {
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
  virtual void PerformCommit(au::SharedPointer<C> c, std::string commit_command, int version,
                             au::ErrorManager *error) = 0;

  // Virtual method to be nofitied when and update comes up
  virtual void NotificationNewModel(int version, au::SharedPointer<C> c) {
  };

  // Get a duplicated version of the model ( node that C is a GPB message )
  au::SharedPointer<C> getCurrentModel() {
    au::TokenTaker tt(&token_);

    return c_;
  }

  // virtual method of samson::zoo::ConnectionWatcherInterface
  virtual void watcher(zoo::Connection *connection, int type, int state, const char *path) {
    au::TokenTaker tt(&token_);               // Mutex protection

    GetDataFromZooNode();                     // Recover data from zk
  };

  // Commit a new commit_command
  void Commit(const std::string& caller, const std::string& commit_command, au::ErrorManager *error) {
    // Perform real commit
    InternCommit(commit_command, error);

    // Log activity for debugging
    if (error->IsActivated())
      last_commits_.push_back(CommitRecord(caller, commit_command, error->GetMessage()));
    else
      last_commits_.push_back(CommitRecord(caller, commit_command, "OK"));

    while (last_commits_.size() > 100) {
      last_commits_.pop_front();
    }
  }

  gpb::Collection *getLastCommitsCollection(const Visualization& visualization) {
    gpb::Collection *collection = new gpb::Collection();

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
      num++;
    }

    return collection;
  }

private:

  void GetDataFromZooNode() {
    // New candidate for data
    au::SharedPointer<C> c(new C());

    // Get a first version of the data model
    while (true) {
      int rc = 0;
      int previous_version = stat_.version;
      {
        // Mutex protection
        au::TokenTaker tt(&token_);
        rc = zoo_connection_->Get(path_.c_str(), this, c.shared_object(), &stat_);
      }

      if (rc) {
        LM_W(("Not possible to get node %s from zk: %s"
              , path_.c_str()
              , samson::zoo::str_error(rc).c_str()));
        sleep(1);
      } else {
        // Keep this new data model
        c_ = c;

        // We have received information correctly
        if (stat_.version > previous_version) {
          NotificationNewModel(stat_.version, c_);
        }
        break;
      }
    }
  }

  void InternCommit(std::string commit_command, au::ErrorManager *error) {
    // Mutex protection
    au::TokenTaker tt(&token_);

    int trial = 0;

    while (true) {
      trial++;

      // Get data from zk
      GetDataFromZooNode();

      // Real changes on data model
      PerformCommit(c_, commit_command, stat_.version, error);

      if (error->IsActivated()) {
        GetDataFromZooNode();    // Model is recovered from zk to make sure it is not affected
        return;                  // No commit is done
      }

      // Try to commit
      int rc = zoo_connection_->Set(path_.c_str(), c_.shared_object(),  stat_.version);

      if (rc == ZBADVERSION) {
        // Wrong vertion ( this means another commit was accepted first )
        LM_W(("Wrong version of zk path %s (%lu)",  path_.c_str(), stat_.version ));
        GetDataFromZooNode();
        continue;
      }

      if (rc) {
        LM_W(("Unexpected error updating node %s: %s ", path_.c_str(), zoo::str_error(rc).c_str()));
        usleep(100000);
        continue;
      } else {
        break;
      }
    }

    // Recover data from zk to get the last version
    GetDataFromZooNode();
  }

  Stat stat_;                              // zk state for data node
  au::SharedPointer<C> c_;                 // Current data
  std::string path_;                       // Path to commit in the zk
  zoo::Connection *zoo_connection_;        // zk conneciton to keep sync
  au::Token token_;                        // Mutex protection

  std::list<CommitRecord> last_commits_;
};
};

#endif  // ifndef _H_SAMSON_ZOO_NODE_COMITTER
