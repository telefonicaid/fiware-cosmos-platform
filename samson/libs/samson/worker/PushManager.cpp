#include "samson/network/Packet.h"
#include "samson/network/Packet.h"
#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/zoo/CommitCommand.h"

#include "PushManager.h"  // Own interface

namespace samson { namespace worker {
                   PushOperation::PushOperation(SamsonWorker *samson_worker
                                                , size_t block_id
                                                , size_t delilah_id
                                                , size_t push_id
                                                , engine::BufferPointer buffer
                                                , const std::vector<std::string>& queues) {
                     samson_worker_ = samson_worker;

                     block_id_   = block_id;
                     delilah_id_ = delilah_id;
                     push_id_    = push_id;
                     for (size_t i = 0; i < queues.size(); i++) {
                       queues_.push_back(queues[i]);
                     }

                     // Copy the header of this block ( for keep information )
                     memcpy(&header, buffer->getData(), sizeof( KVHeader));

                     // Receive notifications when a block has been correctly distributed
                     listen("notification_block_correctly_distributed");

                     // It is not finish by default
                     distributed = false;
                     commited = false;
                   }

                   size_t PushOperation::get_delilah_id() {
                     return delilah_id_;
                   }

                   size_t PushOperation::get_push_id() {
                     return push_id_;
                   }

                   void PushOperation::notify(engine::Notification *notification) {
                     if (distributed) {
                       return;  // If the block has already been distributed, ignore notifications
                     }
                     // Get block_id that has been distributed
                     size_t block_id = notification->environment().Get("block_id", (size_t)-1);

                     // If this is me
                     if (block_id != (size_t)-1) {
                       if (block_id == block_id_) {
                         // Send a message to delilah
                         PacketPointer packet(new Packet(Message::PushBlockResponse));
                         packet->to = NodeIdentifier(DelilahNode, delilah_id_);
                         packet->message->set_push_id(push_id_);
                         samson_worker_->network()->Send(packet);

                         // Set as distributed
                         distributed = true;
                       }
                     }
                   }

                   void PushOperation::commit() {
                     if (!distributed) {
                       LM_W(("Received a commit in a Push operation wihtout distributing previously. Ignoring... "));
                       return;
                     }

                     if (commited) {
                       LM_W(("Received a double commit in a Push operation. Ignoring... "));
                       return;
                     }

                     // Prepare commit command
                     CommitCommand commit_command;
                     for (size_t i = 0; i < queues_.size(); i++) {
                       commit_command.AddBlock(queues_[i], block_id_, header.getKVFormat(), header.range, header.info);
                     }
                     std::string command = commit_command.GetCommitCommand();

                     // Commit
                     au::ErrorManager error;
                     std::string caller = au::str("PushOperation from delilah %s ( push id %lu )"
                                                  , au::code64_str(delilah_id_).c_str()
                                                  , push_id_);

                     samson_worker_->data_model->Commit(caller, command, &error);

                     if (error.IsActivated()) {
                       LM_W(("Error commiting to data model in push operation: %s", error.GetMessage().c_str()));  // Send a commit response message to delilah
                     }
                     PacketPointer packet(new Packet(Message::PushBlockCommitResponse));
                     packet->to = NodeIdentifier(DelilahNode, delilah_id_);
                     packet->message->set_push_id(push_id_);
                     samson_worker_->network()->Send(packet);

                     // Flag as commited
                     commited = true;
                   }

                   std::string PushOperation::getStrIdentifiers() {
                     return au::str("%s ( puhs_id %lu )"
                                    , au::code64_str(delilah_id_).c_str()
                                    , push_id_);
                   }

                   size_t PushOperation::get_block_id() {
                     return block_id_;
                   }

                   std::string PushOperation::getStatus() {
                     if (!distributed) {
                       return "Distributing...";
                     }
                     if (commited) {
                       return "Comitted";
                     }

                     return "Distributed. Pending commit...";
                   }

                   std::string PushOperation::getStrBufferInfo() {
                     return header.str();
                   }

                   // Received a message from a delilah
                   void PushManager::receive_push_block(size_t delilah_id
                                                        , size_t push_id
                                                        , engine::BufferPointer buffer
                                                        , const std::vector<std::string>& queues) {
                     if (buffer == NULL) {
                       LM_W(("Push message without a buffer. This is an error..."));
                       return;
                     }

                     // Modify incomming buffer to assign only one hg
                     KVRanges ranges = samson_worker_->worker_controller->GetMyKVRanges();
                     int hg = ranges.RandomHashGroup();

                     KVHeader *header = (KVHeader *)buffer->getData();
                     if (!header->isTxt() || !header->check() || !header->range.isValid()) {
                       LM_W(("Push message with a non-valid buffer.Ignoring..."));
                       return;
                     }

                     header->range.set(hg, hg + 1);


                     // Create a new block in this worker ( and start distribution process )
                     size_t block_id   = samson_worker_->distribution_blocks_manager->CreateBlock(buffer);


                     // Check valid header
                     if (buffer->getSize() < sizeof(KVHeader)) {
                       LM_W(("Push message with a non-valid buffer.Ignoring..."));
                       return;
                     }

                     // Create PushOpertion object and insert in the vector of pending push operations
                     PushOperation *push_operation =
                       new PushOperation(samson_worker_, block_id, delilah_id, push_id, buffer,
                                         queues);
                     push_operations_.push_back(push_operation);

                     LM_TODO(("If the push is ready to confirm distirbution, answer here"));

                     return;
                   }

                   void PushManager::receive_push_block_commit(size_t delilah_id, size_t push_id) {
                     for (size_t i = 0; i < push_operations_.size(); i++) {
                       // Check if thi is the one
                       if (push_operations_[i]->get_delilah_id() == delilah_id) {
                         if (push_operations_[i]->get_push_id() == push_id) {
                           push_operations_[i]->commit();
                           return;
                         }
                       }
                     }

                     LM_W(("Unused commit message for a push operation"));
                     return;
                   }

                   gpb::Collection *PushManager::getCollectionForPushOperations(const Visualization& visualization) {
                     gpb::Collection *collection = new gpb::Collection();

                     collection->set_name("push operations");

                     for (size_t i = 0; i < push_operations_.size(); i++) {
                       PushOperation *push_operation = push_operations_[i];

                       gpb::CollectionRecord *record = collection->add_record();

                       ::samson::add(record, "Delilah", push_operation->getStrIdentifiers());
                       ::samson::add(record, "block_id", push_operation->get_block_id());
                       ::samson::add(record, "Buffer", push_operation->getStrBufferInfo());
                       ::samson::add(record, "Status", push_operation->getStatus());
                     }

                     return collection;
                   }
                   } }