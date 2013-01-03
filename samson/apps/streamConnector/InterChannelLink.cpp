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

#include "InterChannelLink.h"

namespace stream_connector {
InterChannelLink::InterChannelLink(std::string name
                                   , au::SocketConnection *socket_connection
                                   , au::network::PacketReaderInteface<InterChannelPacket> *interface
                                   ) {
  // Keep for connection information
  socket_connection_ = socket_connection;

  // Init the reader writer
  packet_reader_writer = new au::network::PacketReaderWriter<InterChannelPacket>(name
                                                                                 , socket_connection
                                                                                 , interface);
}

InterChannelLink::~InterChannelLink() {
  // Stop all involved threads
  packet_reader_writer->stop_threads();

  // Delete reader/writer
  delete packet_reader_writer;
}

// Push a packet
void InterChannelLink::push(au::SharedPointer<InterChannelPacket> packet) {
  packet_reader_writer->push(packet);
}

void InterChannelLink::push(const au::Queue<InterChannelPacket>& packets) {
  packet_reader_writer->push(packets);
}

// Return If this can be removed looking at threads
bool InterChannelLink::isRunning() {
  return packet_reader_writer->isRunning();
}

bool InterChannelLink::isConnected() {
  return packet_reader_writer->isConnected();
}

std::string InterChannelLink::host_and_port() {
  return socket_connection_->host_and_port();
}

// Cancel connection
void InterChannelLink::close_socket_connection() {
  socket_connection_->Close();
}

// Stop all threads
void InterChannelLink::close_and_stop() {
  socket_connection_->Close();
  packet_reader_writer->stop_threads();
}

size_t InterChannelLink::bufferedSize() {
  return packet_reader_writer->getOutputBufferedSize();
}

void InterChannelLink::extract_pending_packets(au::Queue<InterChannelPacket>& packets) {
  packet_reader_writer->extract_pending_packets(packets);
}
}