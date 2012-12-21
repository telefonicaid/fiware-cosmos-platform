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
#ifndef _H_STREAM_CONNECTOR_ITEM
#define _H_STREAM_CONNECTOR_ITEM

#include "au/containers/map.h"

#include "engine/Buffer.h"

#include "samson/client/SamsonClient.h"

#include "BufferProcessor.h"
#include "TrafficStatistics.h"
#include "common.h"

namespace stream_connector {
class Connection;
class Channel;
class InputInterChannelConnection;

class Adaptor {
public:

  Adaptor (Channel *_channel, ConnectionType _type, std::string description);
  virtual ~Adaptor();

  /**
   * \brief Start adaptor activity
   */
  virtual void start_item() {
  }

  /**
   * \brief Review this adaptor ( called periodically )
   */
  virtual void review_item() {
  }

  /**
   * \brief Stop adaptor activity
   *
   * Stop all threads to be deleted
   */
  virtual void stop_item() {
  };

  /**
   * \brief Get a status string for this adaptor
   */
  virtual std::string getStatus() {
    return "";
  }

  // Check if we accept a particular inter channel connection
  virtual bool accept(InputInterChannelConnection *connection);

  // Add a connection
  void add(Connection *Connection);

  // Push a new buffer to all connections
  void push(engine::BufferPointer buffer);

  // Reviewing item and all associated connections
  void review();

  // Item management
  void init_item();
  void cancel_item();

  // Getting information
  int getNumConnections();
  ConnectionType getType() const;
  std::string getName();
  std::string getFullName();
  std::string getDescription();
  const char *getTypeStr();
  size_t getConnectionsBufferedSize();

  // Finish managemnt
  void set_as_finished();                                              // Mark this element as finished
                                                                       // ( can be removed by remove_finished_* commands )
  bool is_finished();                                                  // Check if the connection is finished
  void remove_finished_connections(au::ErrorManager *error);           // Remove connections that are already finished

  // Report size of data managed by this adaptor
  void report_output_size(size_t size);
  void report_input_size(size_t size);

  // get first connection
  Connection *getFirstConnection();

private:

  // Mutex protextion
  au::Token token;

  // Name describing this item
  std::string name_;                    // Assigned when added to a channel
  std::string description_;             // Description of this element

  // Collection of connections
  int next_id;       // Next identifier
  au::map<int, Connection> connections;

  // Statistic information
  TrafficStatistics traffic_statistics;

  // Channel pointer
  Channel *channel;

  // Type of channel
  ConnectionType type;

  friend class Connection;
  friend class Channel;
  friend class StreamConnector;

  bool canceled;       // Flag to indicate this is canceled ( not call review again )
  bool finished;       // Flag to indicate this component is finished
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_ITEM
