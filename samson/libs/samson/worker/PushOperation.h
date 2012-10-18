#ifndef _H_SAMSON_PUSH_OPERATION
#define _H_SAMSON_PUSH_OPERATION

#include <string>
#include <vector>

#include "au/statistics/Cronometer.h"

#include "engine/Buffer.h"
#include "engine/Notification.h"
#include "engine/NotificationListener.h"
#include "samson/common/KVHeader.h"
#include "samson/common/Visualitzation.h"


namespace samson {
class NetworkInterface;
class Info;
class Packet;
class SamsonWorker;
}

namespace samson {
class PushOperation : public engine::NotificationListener {
  public:
    PushOperation(SamsonWorker *samson_worker, size_t block_id, size_t block_size, size_t delilah_id, size_t push_id,
                  engine::BufferPointer buffer, const std::vector<std::string>& queues);

    // engine::NotificationListener
    virtual void notify(engine::Notification *notification);

    // Commit order from delilah
    void commit();

    // Get information about this push operation ( worker side )
    std::string getStrIdentifiers() const;
    size_t get_block_id() const;
    size_t get_delilah_id() const;
    size_t get_push_id() const;
    std::string getStrBufferInfo() const;
    std::string getStatus() const;

    // Create packet to be sent to delilah client
    Packet *response_packet(bool commit);

    // Creation time ( to remove old elements )
    size_t time() const;

  private:
    ::samson::SamsonWorker * samson_worker_;   // Pointer to samson worker

    size_t block_id_;   // Identifier of the block created ( given by BLockManager )
    size_t block_size_;   // Size of the created block
    size_t delilah_id_;   // Identifier of deliah node that should be notified when finish
    size_t push_id_;   // Identifier of the concrete push in this delilah
    KVHeader header;   // Buffer information ( for table visualization )
    std::vector<std::string> queues_;   // Vector of queues to add this block

    bool distributed_;   // Flag to indicate that this block has been correctly distributed
    bool commited_;   // Flag to indicate that this block gas been correctly committed

    au::Cronometer cronometer_;
};
}   // End of namespace samson

#endif  // ifndef _H_SAMSON_PUSH_OPERATION
