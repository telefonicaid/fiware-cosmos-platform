

#include "au/ThreadManager.h"
#include "au/network/SocketConnection.h"

#include "SamsonPushConnectionsManager.h"  // Own interface
#include "samson/client/SamsonClient.h"         // samson::SamsonClient


extern size_t buffer_size;
extern samson::SamsonClient *samson_client;
extern char queue_name[1024];

void *run_SamsonPushConnection(void *p) {
  SamsonPushConnection *connection = ( SamsonPushConnection * )p;

  connection->run();
  return NULL;
}

SamsonPushConnection::SamsonPushConnection(au::SocketConnection *_socket_connetion) {
  socket_connetion = _socket_connetion;
  thread_running = true;

  // Alloc buffer to receive data
  buffer = (char * )malloc(buffer_size);

  // Create the push buffer to send data
  pushBuffer = new samson::SamsonPushBuffer(samson_client, queue_name);

  // Create the thread
  pthread_t t;
  au::ThreadManager::shared()->addThread("SamsonPushConnection", &t, NULL, run_SamsonPushConnection, this);
}

SamsonPushConnection::~SamsonPushConnection() {
  delete pushBuffer;
  free(buffer);
}

void SamsonPushConnection::run() {
  while (true) {
    if (socket_connetion->IsClosed()) {
      thread_running = false;
      return;
    }

    // Read and push...
    size_t read_size;
    au::Status s = socket_connetion->partRead(buffer, buffer_size, "SamsonPushConnection", 100, &read_size);

    // Pushding this data to SAMSON system
    pushBuffer->push(buffer, read_size, true);

    if (s != au::OK) {
      // Just to make sure we close
      socket_connetion->Close();
      thread_running = false;
      return;
    }
  }
}