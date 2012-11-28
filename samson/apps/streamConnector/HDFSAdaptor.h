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

#ifndef _H_STREAM_CONNECTOR_HDFS_ADAPTOR
#define _H_STREAM_CONNECTOR_HDFS_ADAPTOR



#include "SingleConnectionAdaptor.h"  // parent class SingleConnectionAdaptor

namespace stream_connector {
class HDFSAdaptor : public SingleConnectionAdaptor {
  std::string host_;
  std::string directory_;

public:

  HDFSAdaptor(Channel *channel, ConnectionType type, const std::string& host, const std::string& directory)
    : SingleConnectionAdaptor(channel, type, au::str("HDFS(%s:%s)", host.c_str(), directory.c_str())) {
    // Keep host and firectory to establish connection latter
    host_ = host;
    directory_ = directory;
  }

  virtual void start_connection() {
    // Method to start the connection
    // This method is called once
    // Run background threads if necessary ( using au::ThreadManager please )

    LOG_SW(("start_connection in HDFS connections is still not defined"));
  }

  virtual void review_connection() {
    // This method is periodically called if you have to do any periodic review
    // This method has to return quickly ( no heavy operations are permitted )
    LOG_SW(("review in HDFS: Pending %s to be sent ", au::str(bufferedSize(), "B").c_str()));


    // How to extract data to be sent ( this has not to be done here )
    while (true) {
      engine::BufferPointer buffer = getNextBufferToSent();

      if (buffer != NULL) {
        LOG_SW(("Sending buffer of %s", au::str(buffer->size(), "B").c_str()));
      } else {
        break;
      }
    }
  }

  // Virtual methods of Connection
  virtual std::string getStatus() {
    return "Inform about internal state here!";
  }

  virtual void stop_connection() {
    // Stop all background threads ( and make sure they do not start again ; ))
    // Alert: Once this method return, streamConnector can remove this object
    LOG_SW(("stop_connection in HDFS connection is still not defined"));
  }
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_HDFS_ADAPTOR

