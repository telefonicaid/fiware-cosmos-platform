/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
#include "samson/worker/CommitCommand.h"
#include "zoo/common.h"
#include "zoo/Connection.h"

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

namespace au {
struct CommitRecord {
    int id;
    std::string caller;
    std::string commit_command;
    std::string result;
    double commit_time;

    CommitRecord(int _id
                 , const std::string& _caller
                 , const std::string& _commit_command
                 , const std::string& _result
                 , double _commit_time) {
      id = _id;
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
  
        // No version when eveything start
        version_ = -1;
        
        // Identifiers for the list of commits
        last_commits_id_ = 1;
    }
  

    virtual ~ZooNodeCommiter() {
    }

    // Virtual function to update internal state with a provided commit
    virtual void
    PerformCommit(au::SharedPointer<C> c, std::string commit_command, int version, au::ErrorManager& error) = 0;

    // Virtual method to be nofitied when and update comes up
    virtual void NotificationNewModel(int previous_version
                                      , au::SharedPointer<C> previous_data
                                      , int version
                                      , au::SharedPointer<C> new_data ) {
    }

    // Get current data model
    au::SharedPointer<C> getCurrentModel() {
      au::TokenTaker tt(&token_);
      return c_;
    }

  // Get current data model
  au::SharedPointer<C> getDuplicatedCurrentModel() {
    au::TokenTaker tt(&token_);
    au::SharedPointer<C> c( new C() );
    c->CopyFrom( *c_.shared_object() );
    return c;
  }

    // virtual method of engine::NotificationListener
    virtual void notify(engine::Notification *notification) {
      au::TokenTaker tt(&token_);   // Mutex protection
      GetDataFromZooNode();   // Recover data from zk
    }

    // Commit a new commit_command
    void Commit(const std::string& caller, const std::string& commit_command, au::ErrorManager& error) {

      // Perform real commit
      au::Cronometer cronometer;
      InternCommit(commit_command, error);
      double time = cronometer.seconds();
      
      // Log activity for debugging
      if (error.IsActivated()) {
        last_commits_.push_front(CommitRecord(last_commits_id_++,caller, commit_command, error.GetMessage() , time));
      } else {
        last_commits_.push_front(CommitRecord(last_commits_id_++,caller, commit_command, "OK" , time));
      }

      while (last_commits_.size() > 100) {
        last_commits_.pop_back();
      }
    }

  int version()
  {
    return version_;
  }
  
protected:
  
  std::list<CommitRecord> last_commits_;

  private:
  
    int GetDataFromZooNode() {
      
      // New candidate for data
      au::SharedPointer<C> c(new C());

      int rc = zoo_connection_->WaitUntilConnected(5000);
      if (rc) {
        return rc;
      }

      Stat stat;   // zk state for data node

      {
        au::TokenTaker tt(&token_);   // Mutex protection
        rc = zoo_connection_->Get(path_.c_str(), engine_id(), c.shared_object(), &stat);
      }
      
      if (rc) {
        LOG_W( logs.zoo , ("Not possible to get node %s from zk: %s"
                , path_.c_str()
                , zoo::str_error(rc).c_str()));
        
        
        if (( rc == ZC_ERROR_GPB ) || ( rc == ZC_ERROR_GPB_NO_INITIALIZED ))
        {
          LOG_E(logs.zoo, ("Serialitzation errors are not allowed in DataCommiter"));
          LM_X(1, ("Fatal error: Serialitzation error in Data commiter for path %s" , path_.c_str() ));
        }
        return rc;
      }

      // Keep this new data model
      LOG_M( logs.zoo , ("Update data model in path %s from %d to %d" , path_.c_str() , version_, stat.version ));

      if( version_ > stat.version )
      {
        LOG_E(logs.zoo, ("Major error in data model commiter at path %s. Previous version %d Current Version %d"
                         , path_.c_str()
                         , version_
                         , stat.version ));
        
        // Change internal state
        c_ = c;
        version_ = stat.version;
        return 0;
      }
      
      // Check if we have a new version now...
      if( stat.version > version_ )
      {
        // Notification for the children classes
        NotificationNewModel( version_ , c_ , stat.version , c );

        // Change internal state
        c_ = c;
        version_ = stat.version;
      }
      
      return 0;   // Everything ok
    }

    void InternCommit(std::string commit_command, au::ErrorManager& error) {

      // Mutex protection
      au::TokenTaker tt(&token_);

      int trial = 0;

      while (true) {
        trial++;

        // If not previous data, load from ZK
        if( c_ == NULL ) {
          LOG_M( logs.zoo , ("Getting data since no previous model" ));
          int rc = GetDataFromZooNode();   // Get data from zk
          if (rc) {
            error.set(au::str("Error with ZK: %s", zoo::str_error(rc).c_str()));
            return;
          }
        }

        // Get a copy of the data model
        au::SharedPointer<C> c = getDuplicatedCurrentModel();
        
        LOG_M( logs.zoo , ("Performing commir %s over path %s" , commit_command.c_str() , path_.c_str() ));
        PerformCommit(c, commit_command, version_, error);   // Real changes on data model

        if (error.IsActivated()) {
          return; // If error in the operation itself, No commit is done at all
        }

        // Try to commit
        int rc = zoo_connection_->Set(path_.c_str(), c.shared_object(), version_ );

        if (rc == ZBADVERSION) {
          
          LOG_M( logs.zoo , ("%d is the wrong version for %s: This mean another worker has commited first"
                             , version_
                             , path_.c_str() ));
          
          // Wrong version ( this means another commit was accepted first )
          int previous_version = version_;
          int rc = GetDataFromZooNode();   // Get data from zk
          if (rc) {
            error.set(au::str("Error with ZK: %s", zoo::str_error(rc).c_str()));
            return;
          }
          
          if( version_ <= previous_version )
          {
            error.set(au::str("Error in DataCommit: Version %d was rejected and not updated from ZK ( new version %d )"
                              , previous_version, version_ ));
            return;
          }
          
          continue;   // Loop to load again data model and commit
        }

        if (rc) {
          // Any other error cancel this operation
          error.set(au::str("Error with ZK: %s ", zoo::str_error(rc).c_str()));
          return;
        }

        // Operation is commited correctly
        break;
      }

      // Recover data from zk to get the last version
      GetDataFromZooNode();
    }

    au::SharedPointer<C> c_;            // Current data
    std::string path_;                  // Path to commit in the zk
    zoo::Connection *zoo_connection_;   // zk conneciton to keep sync
    au::Token token_;                   // Mutex protection
    int version_;

    int last_commits_id_;
};
}

#endif  // ifndef _H_SAMSON_ZOO_NODE_COMITTER
