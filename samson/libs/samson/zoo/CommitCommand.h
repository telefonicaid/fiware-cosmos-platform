
#ifndef _H_SAMSON_COMMIT_COMMAND
#define _H_SAMSON_COMMIT_COMMAND

#include <string>
#include <vector>

#include "au/CommandLine.h"
#include "au/ErrorManager.h"

#include "samson/common/KVInfo.h"
#include "samson/common/KVRange.h"
#include "samson/module/KVFormat.h"

namespace samson {
// ------------------------------------------------------------------
//
// class CommitCommandItem
//
// Item inside CommitCommand ( atomic operation over a queue )
// Thist class accumuate information about a queue operation commit
// and generte the std::string to be included in a commit command over DataModel
// ------------------------------------------------------------------


class CommitCommandItem {
  std::string command_;
  std::string queue_;
  size_t block_id_;
  KVFormat format_;
  KVRange range_;
  KVInfo info_;

  /*
   *
   * FORMAT  command:queue:block_id:key_format:value_format:range_begin:range_end:info.kvs:info.size
   *
   * [0]  command
   * [1]  queue
   * [2]  block_id
   * [3][4] format
   * [5][6] range
   * [7][8] info ( size - kvs )
   *
   */

public:

  CommitCommandItem(const std::string& command
                    , const std::string& queue
                    , size_t block_id
                    , const KVFormat& format
                    , const KVRange& range
                    , const KVInfo& info);

  // Parse a provided command to generate a new item
  static CommitCommandItem *create_item(const std::string& command, au::ErrorManager *error);

  // Member accessors
  std::string command() const;
  std::string queue() const;
  size_t block_id() const;
  KVFormat format() const;
  KVRange range() const;
  KVInfo info() const;

  // Generate string command to be added to others and forma  complete commit command for Datamodel
  std::string str() const;
};


class CommitCommand {
  // Vector of items
  au::vector<CommitCommandItem> items_;

public:

  CommitCommand();
  ~CommitCommand();

  // Parse a received commit-command string to generate items
  void ParseCommitCommand(const std::string& command, au::ErrorManager *error);

  // handy function to add items to this command
  void AddBlock(const std::string& queue
                , size_t block_id
                , const KVFormat& format
                , const KVRange& range
                , const KVInfo& info);
  void RemoveBlock(const std::string& queue
                   , size_t block_id
                   , const KVFormat& format
                   , const KVRange& range
                   , const KVInfo& info);

  // Generate commit-command string based on all provided items
  std::string GetCommitCommand() const;

  // Accessor
  const au::vector<CommitCommandItem>& items();
};
}

#endif // ifndef _H_SAMSON_COMMIT_COMMAND