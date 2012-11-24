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




#ifndef _H_AU_NETWORK_PACKET_READER_WRITER
#define _H_AU_NETWORK_PACKET_READER_WRITER

#include "au/statistics/Cronometer.h"

#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/network/FileDescriptor.h"

namespace au {
namespace network {
/*
 *
 * Note:
 *
 * All clases and interfaces here assume "P"...
 *
 * implemented methods au::Status read( FileDescriptor* fd )
 * implemented methods au::Status write( FileDescriptor* fd )
 *
 */

// Interface to read
template <class P>                 // Class for the packet
class PacketReaderInteface {
public:
  virtual void process_packet(au::SharedPointer<P> packet) = 0;
};

template <class P>                 // Class for the packet
class PacketReader : public au::Thread {
  au::FileDescriptor *fd_;
  PacketReaderInteface<P> *interface_;

public:

  PacketReader(std::string name, au::FileDescriptor *fd, PacketReaderInteface<P> *interface)
    : Thread(name) {
    fd_ = fd;
    interface_ = interface;

    // Start me as a thread
    StartThread();
  }

  virtual ~PacketReader() {
  }

  virtual void RunThread() {
    while (true) {
      if (IsThreadQuiting()) {
        return;
      }

      if (fd_->IsClosed()) {
        return;                 // End of thread since socket is disconnected
      }
      au::SharedPointer<P> packet(new P());
      au::Status s = packet->read(fd_);

      if (s != au::OK) {
        // Close connection
        fd_->Close();

        return;                 // End of the tread
      }

      // Execute this receive method in the interface
      interface_->process_packet(packet);
    }
  }
};

template <class P>                 // Class for the packet
class PacketWriter : public au::Thread {
  au::FileDescriptor *fd_;

  au::Token token;                 // Token to protect the list
  Queue<P> packets_;

  // Size accumulated to be sent
  size_t buffered_size;

public:

  PacketWriter(std::string name, au::FileDescriptor *fd)
    : Thread(name), token("PacketWriter") {
    fd_ = fd;
    buffered_size = 0;

    // Start me as a thread
    StartThread();
  }

  virtual ~PacketWriter() {
  }

  virtual void RunThread() {
    while (true) {
      if (IsThreadQuiting()) {
        return;
      }


      if (fd_->IsClosed()) {
        return;                    // End of thread since socket is disconnected
      }
      // Recover next packet if any
      au::SharedPointer<P> packet;
      {
        au::TokenTaker tt(&token);  // Mutex protection
        if (packets_.size() > 0) {
          packet = packets_.Front();
        }
      }

      if (packet != NULL) {
        // Recover the packet to be send ( do not remove from list )
        au::Status s = packet->write(fd_);

        if (s != au::OK) {
          return;  // End of thread since packet could not be sent
        }
        // Release the packet from the list
        {
          au::TokenTaker tt(&token);                 // Mutex protection
          packets_.Pop();
          buffered_size -= packet->getSize();
        }
      } else {
        // TODO: This could be implemented in a better blocking way
        usleep(100000);
      }
    }
  }

  void extract_pending_packets(Queue<P>& queue) {
    queue.Push(packets_);
  }

  void push(au::SharedPointer<P> packet) {
    au::TokenTaker tt(&token);                 // Mutex protection

    packets_.Push(packet);
    buffered_size += packet->getSize();
  }

  void push(au::Queue<P> packets) {
    au::TokenTaker tt(&token);                 // Mutex protection

    std::vector< au::SharedPointer<P> > items = packets.items();
    for (size_t i = 0; i < items.size(); i++) {
      packets_.Push(items[i]);
      buffered_size += items[i]->getSize();
    }
  }

  size_t bufferedSize() {
    return buffered_size;
  }
};


template <class P>                 // Class for the packet
class PacketReaderWriter {
  // Keep a pointer to fg to delete at the end
  au::FileDescriptor *fd_;

  PacketReader<P> *packet_reader_;
  PacketWriter<P> *packet_writer_;

public:

  PacketReaderWriter(std::string name, au::FileDescriptor *fd, PacketReaderInteface<P> *interface) {
    fd_ = fd;
    packet_reader_ = new PacketReader<P>(name + "_reader", fd, interface);
    packet_writer_ = new PacketWriter<P>(name + "_writer", fd);
  }

  ~PacketReaderWriter() {
    // Just in case
    stop_threads();

    delete packet_writer_;
    delete packet_reader_;
    delete fd_;
  }

  void close() {
    fd_->Close();
  }

  void push(au::SharedPointer<P> packet) {
    packet_writer_->push(packet);
  }

  void push(au::Queue<P> packets) {
    packet_writer_->push(packets);
  }

  void stop_threads() {
    packet_reader_->StopThread();
    packet_writer_->StopThread();
  }

  bool isRunning() {
    if (packet_reader_->IsThreadRunning()) {
      return true;
    }


    if (packet_writer_->IsThreadRunning()) {
      return true;
    }


    return false;
  }

  bool isConnected() {
    return !fd_->IsClosed();
  }

  size_t getOutputBufferedSize() {
    return packet_writer_->bufferedSize();
  }

  void extract_pending_packets(Queue<P>& queue) {
    packet_writer_->extract_pending_packets(queue);
  }
};
}
}  // End of namespace

#endif  // ifndef _H_AU_NETWORK_PACKET_READER_WRITER
