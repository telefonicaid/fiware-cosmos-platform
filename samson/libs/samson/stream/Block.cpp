
#include "logMsg/logMsg.h"                          // LM_W

#include "au/S.h"

#include "au/mutex/TokenTaker.h"

#include "engine/DiskManager.h"                     // notification_disk_operation_request_response
#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Engine.h"                          // engine::Engine
#include "engine/MemoryManager.h"                   // engine::MemoryManager
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessManager.h"

#include "samson/common/MessagesOperations.h"
// MemoryBlocks
#include "samson/common/KVHeader.h"
#include "samson/common/SamsonSetup.h"              // samson::SamsonSetup

#include "samson/module/KVFormat.h"
#include "samson/module/ModulesManager.h"           // ModulesManager

#include "BlockList.h"                              // BlockList
#include "BlockManager.h"                           // BlockManager

#include "Block.h"                                  // Own interface



namespace samson {
namespace stream {
Block::Block(size_t block_id, engine::BufferPointer buffer) : token_lookupList("token_lookupList") {
  // Keep the buffer in this block
  buffer_ = buffer;

  // Default state is on_memory because the buffer has been given at memory ( input buffer )
  state_ = on_memory;

  // Unique identifier of the block across the cluster
  block_id_ = block_id;

  // Get a copy of the header
  memcpy(&header, buffer->getData(), sizeof(KVHeader));

  // No lookup list by default
  lookupList = NULL;

  // First idea of sort information
  update_sort_information();

  LM_T(LmtBlockManager, ("Block created from buffer: %s", str().c_str()));
}

Block::Block(size_t block_id, KVHeader *_header) : token_lookupList("token_lookupList") {
  // Unique identifier of the block across the cluster
  block_id_ = block_id;

  // Default state is on_memory because the buffer has been given at memory
  state_ = on_disk;

  // Get a copy of the header
  memcpy(&header, _header, sizeof(KVHeader));

  // No llokup list by default
  lookupList = NULL;

  // First idea of sort information
  update_sort_information();

  // Put cronometer to 1 hour before to remove blocks not included in the data model rigth now
  cronometer.AddOffset(-24 * 60 * 60);

  LM_T(LmtBlockManager, ("Block created from id: %s", this->str().c_str()));
}

Block::~Block() {
  if (lookupList) {
    delete lookupList;
    lookupList = NULL;
  }
}

au::SharedPointer<KVFile> Block::getKVFile(au::ErrorManager& error) {
  au::TokenTaker tt(&token_file_);

  if (file_ == NULL) {
    // Create the file...
    if (buffer_ != NULL)
      file_ = KVFile::create(buffer_, error); else
      error.set(au::str("No buffer in memory for block %lu", block_id_));
  }
  return file_;
}

Block::BlockState Block::state() {
  return state_;
}

void Block::update_sort_information() {
  min_task_id_ = (size_t)-1;
  max_priority_ = 0;

  au::set<BlockList>::iterator it;
  for (it = block_lists_.begin(); it != block_lists_.end(); it++) {
    BlockList *block_list = *it;

    size_t task_id = block_list->task_id();
    int priority = block_list->task_id();

    if (( min_task_id_ == (size_t)-1) || ( min_task_id_ > task_id ))
      min_task_id_ = task_id; if (priority > max_priority_)
      max_priority_ = priority;
  }
}

std::string Block::file_name() {
  return SamsonSetup::shared()->blockFileName(block_id_);
}

// Command over the block

void Block::freeBlock() {
  // Remove the lookup table if exist
  {
    au::TokenTaker tt(&token_lookupList);
    if (lookupList) {
      delete lookupList;
      lookupList = NULL;
    }
  }


  if (state_ != ready) {
    LM_W(("No sense to call free to a Block that state != ready "));
    return;
  }

  state_ = on_disk;

  if (buffer_ == NULL) {
    LM_W(("Buffer not present in a ready block"));
    return;
  }

  LM_T(LmtBlockManager, ("destroyBuffer for block:'%s'", str().c_str()));

  // Relase buffer
  buffer_ = NULL;
}

// Get information about this block
void Block::update(BlockInfo &block_info) {
  // Information about number of blocks
  block_info.num_blocks++;

  // Information about sizes
  block_info.size += header.info.size;
  if (is_content_in_memory())
    block_info.size_on_memory += header.info.size; if (is_content_in_disk()) {
    block_info.size_on_disk += header.info.size;  /*
                                                   * if( isLockedInMemory() )
                                                   * block_info.size_locked += size;
                                                   */
  }
  // Key-Value information
  block_info.info.append(header.info);

  block_info.push(getKVFormat());

  block_info.pushTime(header.time);
}

size_t getSize(std::set<Block *> &blocks) {
  size_t total = 0;

  std::set<Block *>::iterator i;
  for (i = blocks.begin(); i != blocks.end(); i++) {
    total += (*i)->getSize();
  }

  return total;
}

std::string Block::str() {
  std::ostringstream output;

  output << "[ ";
  output << " id=" << block_id_ << " size=" << header.info.size << " " <<  header.range.str() << "(" << str_state() <<
  ")";
  output << " ]";
  return output.str();
}

// Function to check if this block can be removed from block manager ( basically it is not contained anywhere )
bool Block::canBeRemoved() {
  if (state_ == reading)
    return false;

  if (state_ == writing)
    return false;

  if (block_lists_.size() > 0)
    return false;

  if (block_lists_lock_.size() > 0)
    return false;

  return true;
}

bool Block::is_content_in_memory() {
  return ((state_ == ready ) || (state_ == on_memory) || ( state_ == writing ));
}

bool Block::is_content_in_disk() {
  return ((state_ == ready ) || (state_ == on_disk) || ( state_ == reading ));
}

size_t Block::getSize() {
  return header.info.size + sizeof(KVHeader);
}

size_t Block::getSizeOnMemory() {
  if (is_content_in_memory())
    return getSize(); else
    return 0;
}

size_t Block::getSizeOnDisk() {
  if (is_content_in_disk())
    return getSize(); else
    return 0;
}

KVRange Block::getKVRange() {
  return header.range;
}

KVInfo Block::getKVInfo() {
  return header.info;
}

size_t Block::get_block_id() {
  return block_id_;
}

KVFormat Block::getKVFormat() {
  return header.getKVFormat();
}

size_t Block::getTime() {
  return header.time;
}

void Block::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  samson::add(record, "block_id", block_id_, "left,different");

  samson::add(record, "size", getSize(), "f=uint64,sum");

  samson::add(record, "KVFormat", getKVFormat().str(), "left,different");
  samson::add(record, "KVRange", getKVRange().str(), "left,different");
  samson::add(record, "KVInfo", getKVInfo().str(), "left,different");

  samson::add(record, "state", str_state(), "left,different");

  samson::add(record, "locked", is_content_locked_in_memory() ? "yes" : "no", "left,different");

  // Next task
  if (min_task_id_ == (size_t)(-1)) {
    samson::add(record, "next task", "none", "left,different");
  } else {
    samson::add(record, "next task", min_task_id_, "left,different");  // Priority level
  }
  samson::add(record, "priority", max_priority_, "left,different");

  // Time of creation
  samson::add(record, "created", au::S(cronometer).str(), "left,different");
}

// au::Token token_lookupList;
// BlockLookupList* lookupList;

std::string Block::lookup(const char *key, std::string outputFormat) {
  // Mutex preotection
  au::TokenTaker tt(&token_lookupList);

  // We should check if the block can be locked in memory...

  if (!lookupList) {
    lookupList = new BlockLookupList(this);

    // Detect error during creating
    if (lookupList->error.IsActivated()) {
      LM_E(("Error creating BlockLookupList (%s)", lookupList->error.GetMessage().c_str()));
      delete lookupList;
      lookupList = NULL;
      return au::xml_simple("error", "Error creating BlockLookupList");
    }
  } else {
    LM_M(("lookupList already created for block_id %lu", block_id_ ));
  }

  return lookupList->lookup(key, outputFormat);
}

std::string Block::str_state() {
  std::ostringstream output;

  // Memory status
  if (is_content_locked_in_memory()) {
    output << "L";
  } else if (is_content_in_memory()) {
    output << "M";
  } else if (state_ == reading) {
    output << "R";
  } else {
    output << " ";  // Disk status
  }
  if (is_content_in_disk())
    output << "D"; else if (state_ == writing)
    output << "W"; else
    output << " "; return output.str();
}

// Get the header
KVHeader Block::getHeader() {
  return header;
}

bool Block::is_content_locked_in_memory() {
  return (block_lists_lock_.size() > 0);
}

void Block::add_block_list(BlockList *list) {
  block_lists_.insert(list);
  update_sort_information();
}

void Block::remove_block_list(BlockList *list) {
  // Remove from all the lists
  block_lists_.erase(list);
  block_lists_lock_.erase(list);
  update_sort_information();
}

// lock content of this block in memory
void Block::lock_content_in_memory(BlockList *list) {
  block_lists_lock_.insert(list);
  update_sort_information();
}

void Block::unlock_content_in_memory(BlockList *list) {
  block_lists_lock_.erase(list);
  update_sort_information();
}

// Function to compare 2 blocks
bool Block::compare(Block *b1, Block *b2) {
  size_t min_task_id_1 = b1->min_task_id_;
  size_t min_task_id_2 = b2->min_task_id_;

  if (min_task_id_1 == min_task_id_2) {
    // Comapre by priority
    int p1 = b1->max_priority_;
    int p2 = b2->max_priority_;


    if (p1 == p2) {
      // Sort by time
      double t1 = b1->cronometer.seconds();
      double t2 = b2->cronometer.seconds();

      return ( t1 < t2 );
    }

    return p1 > p2;
  } else {
    return( min_task_id_1 < min_task_id_2 );
  }
}

engine::BufferPointer Block::buffer() {
  return buffer_;
}

size_t Block::creation_time() {
  return cronometer.seconds();
}
}
}
