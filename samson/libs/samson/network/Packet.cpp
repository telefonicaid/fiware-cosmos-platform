/* *******************************************************************************
 *
 * FILE                     Packet.cpp
 *
 * DESCRIPTION              Definition of the packet to be exchange in the samson-ecosystem
 *
 */

#include "au/string.h"

#include "Packet.h"             // Own interface

namespace samson {
Packet::Packet(Message::MessageCode _msgCode, NodeIdentifier to_, bool disposable_) {
  msgCode    = _msgCode;
  message    = new gpb::Message();
  disposable = disposable_;
  to = to_;
};

Packet::Packet(Packet *p) {
  // Copy the message type
  msgCode    = p->msgCode;
  disposable = p->disposable;

  // Point to the same buffer
  // buffer_container.set_buffer( p->buffer_container.buffer() );
  buffer_ = p->buffer_;

  // Google protocol buffer message
  message = new gpb::Message();
  message->CopyFrom(*p->message);
};

Packet::~Packet() {
  // Buffer contained in buffer_container is auto-released

  delete message;
}

void Packet::set_buffer(engine::BufferPointer buffer) {
  // Handy function to get the buffer
  buffer_ = buffer;
}

engine::BufferPointer Packet::buffer() {
  // Handy function to get the buffer
  return buffer_;
}

au::Status Packet::write(au::FileDescriptor *fd, size_t *size) {
  au::Status s;
  Message::Header header;

  if (size)
    *size = 0; LM_T(LmtSocketConnection, ("Sending Packet '%s' to %s ", str().c_str(), fd->name().c_str()));

  //
  // Preparing header
  //

  memset(&header, 0, sizeof(header));

  header.code        = msgCode;
  header.gbufLen     = message->ByteSize();
  header.kvDataLen   = (buffer_ != NULL) ? buffer_->getSize() : 0;

  // Set magic number
  header.setMagicNumber();

  //
  // Sending header
  //
  s = fd->partWrite(&header, sizeof(header), "header");

  if (size)
    *size += sizeof(header); if (s != au::OK)
    // LM_RE(s, ("partWrite:header(%s): %s", name.c_str(), au::status(s)));
    return s;

  //
  // Sending Google Protocol Buffer
  //
  if (header.gbufLen != 0) {
    char *outputVec;

    outputVec = (char *)malloc(header.gbufLen + 2);
    if (outputVec == NULL)
      LM_XP(1, ("malloc(%d)", header.gbufLen)); if (message->SerializeToArray(outputVec, header.gbufLen) == false)
      LM_X(1, ("SerializeToArray failed")); s = fd->partWrite(outputVec, header.gbufLen, "Google Protocol Buffer");
    free(outputVec);
    if (s != au::OK)
      LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s))); if (size)
      *size += header.gbufLen;
  }

  if (buffer_ != 0) {
    s = fd->partWrite(buffer_->getData(), buffer_->getSize(), "KV data");
    if (s != au::OK)
      LM_RE(s, ("partWrite returned %d and not the expected %d", s, buffer_->getSize())); if (size)
      *size += buffer_->getSize();
  }

  return au::OK;
}

au::Status Packet::read(au::FileDescriptor *fd, size_t *size) {
  au::Status s;
  Message::Header header;

  LM_T(LmtSocketConnection, ("SocketConnection %s: Reading packet", str().c_str()));
  if (size)
    *size = 0; s = fd->partRead(&header, sizeof(Message::Header), "Header", 300);      // Timeout 300 secs for next packet
  if (s != au::OK)
    return s;

  if (size)
    *size += sizeof(Message::Header);  // Check header
  if (!header.check()) {
    fd->Close();   // Close connection ( We close here since it is not a io error, is a protocol error )
    LM_E(("Error checking received header from %s", fd->name().c_str()));
    return au::Error;   // Generic error
  }

  // Get the message code from header
  msgCode = header.code;

  if (header.gbufLen != 0) {
    char *dataP = (char *)calloc(1, header.gbufLen + 1);

    s = fd->partRead(dataP, header.gbufLen, "Google Protocol Buffer", 300);
    if (s != au::OK) {
      free(dataP);
      return s;
    }
    if (size)
      *size += header.gbufLen; LM_T(LmtSocketConnection,
                                    ("Read %d bytes of GOOGLE DATA from '%s'", header.gbufLen, fd->name().c_str()));

    // Decode the google protocol buffer message
    message->ParseFromArray(dataP, header.gbufLen);

    if (message->IsInitialized() == false) {
      LM_E(("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!",
            header.gbufLen, samson::Message::messageCode(header.code)));
      // Close connection ( We close here since it is not a io error, is a protocol error )
      free(dataP);
      fd->Close();
      return au::Error;   // Generic error
    }

    free(dataP);
  }

  if (header.kvDataLen != 0) {
    // Alloc a buffer to read buffer of data
    std::string buffer_name = au::str("Network Buffer from %s", fd->name().c_str());

    au::Cronometer cronometer;
    while (true) {
      double memory_usage = engine::MemoryManager::shared()->memory_usage();
      if (memory_usage < 0.9)
        break; else
        usleep(10000); if (cronometer.seconds() > 1) {
        LM_W(("Cannot create buffer for packet since memory usage is %s"
              , au::str_percentage(memory_usage).c_str()));
        cronometer.Reset();
      }
    }

    buffer_ = engine::Buffer::create(buffer_name, "network", header.kvDataLen);

    char *kvBuf  = buffer_->getData();
    s = fd->partRead(kvBuf, header.kvDataLen, "Key-Value Data", 300);

    if (s != au::OK)
      return s;

    LM_T(LmtSocketConnection, ("Read %d bytes of KV DATA from '%s'", header.kvDataLen, fd->name().c_str()));
    if (size)
      *size += header.kvDataLen; buffer_->setSize(header.kvDataLen);
  }

  return au::OK;
}

Packet *Packet::messagePacket(std::string message) {
  Packet *packet = new Packet(Message::Message);

  packet->message->set_message(message);
  return packet;
}

std::string Packet::str() {
  std::ostringstream output;

  output << "Packet " << messageCode(msgCode);

  // Extra information for worker command
  if (msgCode == Message::WorkerCommand)
    output << "(W-Command: " << message->worker_command().command() << ")"; if (msgCode ==
                                                                                Message::WorkerCommandResponse)
    output << "(W-CommandResponse: " << message->worker_command_response().worker_command().command() << ")";
  if (msgCode ==
      Message
      ::
      ClusterInfoUpdate)
    output << "(ClusterInfoUpdate version " << message->cluster_info_version() << " )";
  if (buffer_ != NULL)
    output << " [ Buffer " << au::str(buffer_->getSize()) << "/" << au::str(buffer_->getMaxSize()) << " ]";
  return output.str();
}

size_t Packet::getSize() {
  size_t total = 0;

  if (buffer_ != NULL)
    total += buffer_->getSize(); total += message->ByteSize();

  return total;
}
}
