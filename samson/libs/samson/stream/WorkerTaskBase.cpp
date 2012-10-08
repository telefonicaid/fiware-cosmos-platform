
#include "samson/common/coding.h"

#include "logMsg/logMsg.h"      // LM_M
#include "samson/worker/SamsonWorker.h"

#include "Block.h"              // samson::Stream::Block
#include "BlockManager.h"       // samson::stream::BlockManager
#include "WorkerTaskBase.h"      // Own interface


namespace samson {
  namespace stream {
    WorkerTaskBase::WorkerTaskBase( ::samson::SamsonWorker* samson_worker , size_t id  , const std::string& concept )
    : block_list_container_(au::str("block lists for task %lu", id), id) {
      samson_worker_ = samson_worker;
      id_ = id;    // Not assigned at the moment
      ready_ = false;   // By default it is not ready
      task_state_ = "Init";    // Set initial state
      concept_ = concept;
      finished_ = false;

      // Enviroment for this tasks
      environment_.Set("system.task_id", id);

    }
    
    WorkerTaskBase::~WorkerTaskBase() {
    }
    
    bool WorkerTaskBase::is_ready() {
      if (ready_) {
        return true;
      }
      
      // Check if all the input blocks are in memory
      ready_ = block_list_container_.is_content_in_memory();
      
      if (ready_) {
        // Lock all content in memory
        block_list_container_.lock_content_in_memory();
        
        // Change state to ready...
        SetTaskState("ready");
      }
      
      return ready_;
    }
    
    size_t WorkerTaskBase::worker_task_id() const{
      return id_;
    }
    
    std::string WorkerTaskBase::concept() const{
      return concept_;
    }
    
    void WorkerTaskBase::SetTaskState(const std::string& task_state) {
      task_state_ = task_state;
    }
    
    std::string WorkerTaskBase::task_state() {
      return task_state_;
    }
    
    void WorkerTaskBase::AddInput(int channel, BlockPointer block, KVRange range, KVInfo info) {
      std::string block_list_name = au::str("input_%d", channel);
      BlockList *block_list = block_list_container_.getBlockList(block_list_name);
      
      block_list->add(new BlockRef(block, range, info));
    }
    
    void WorkerTaskBase::AddOutput(int channel, BlockPointer block, KVRange range, KVInfo info) {
      std::string block_list_name = au::str("output_%d", channel);
      BlockList *block_list = block_list_container_.getBlockList(block_list_name);
      
      block_list->add(new BlockRef(block, range, info));
      
      // Keep the list of generated output blocks
      output_block_ids_.push_back( block->block_id() );
    }
    
    void WorkerTaskBase::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
      
      if (visualization.get_flag("data")) {
        add(record, "id", worker_task_id(), "left,different");
        for (int i = 0; i < 3; i++) {
          BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
          BlockInfo block_info = block_list->getBlockInfo();
          add(record, au::str("Input %d", i), block_info.strShort(), "left,different");
        }
        for (int i = 0; i < 3; i++) {
          BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
          BlockInfo block_info = block_list->getBlockInfo();
          add(record, au::str("Output %d", i), block_info.strShort(), "left,different");
        }
        
        return;
      }
      
      if (visualization.get_flag("blocks")) {
        add(record, "id", id_, "left,different");
        
        for (int i = 0; i < 3; i++) {
          BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
          add(record, au::str("Input %d", i), block_list->str_blocks(), "left,different");
        }
        for (int i = 0; i < 3; i++) {
          BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
          add(record, au::str("Output %d", i), block_list->str_blocks(), "left,different");
        }
        
        return;
      }
      
      
      add(record, "id", worker_task_id(), "left,different");
      add(record, "concept", concept(), "left,different");
      
      add(record, "waiting", ProcessItem::waiting_time_seconds(), "f=time,different");
      add(record, "running ", ProcessItem::running_time_seconds(), "f=time,different");
      add(record, "progress ", progress(), "f=percentadge,different");

      if(finished_)
        add(record, "finished ", "Yes", "different");
      else
        add(record, "finished ", "No", "different");

      add(record, "error ", error().GetMessage(), "different");
      
      if (ProcessItem::finished()) {
        add(record, "state", "finished", "left,different");
      } else if (ProcessItem::running()) {
        add(record, "state", "running", "left,different");
      } else {
        add(record, "state", task_state(), "left,different");
      }
            
      
      /*
       * // Collect information from inputs / outputs
       * add( record , "input_0" , getBlockList("input_0")->getBlockInfo().strShort() , "different");
       * add( record , "all_inputs" , getUniqueBlockInfo().strShort()  , "different");
       */
    }
    
    bool WorkerTaskBase::finished() const {
      
      if( !finished_ )
        return false;
      
      // We should check here that all blocks generated by this tasks are really distributed
      for ( size_t i = 0 ; i < output_block_ids_.size() ; i ++ )
        if( samson_worker_->worker_block_manager()->IsBlockBeingDistributed( output_block_ids_[i] ) )
          return false;
      
      return true;
    }

    std::string WorkerTaskBase::str_inputs()
    {
      std::ostringstream output;
      for (int i = 0; i < 3; i++) {
        BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
        BlockInfo block_info = block_list->getBlockInfo();
        output << block_info.strShortInfo() << " ";
      }
      return output.str();
    }
    std::string WorkerTaskBase::str_outputs()
    {
      std::ostringstream output;
      for (int i = 0; i < 3; i++) {
        BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
        BlockInfo block_info = block_list->getBlockInfo();
        output << block_info.strShortInfo() << " ";
      }
      return output.str();
    }
    
    
    void WorkerTaskBase::SetFinished() {
      finished_ = true;
    }
    
    void WorkerTaskBase::SetFinishedWithError(const std::string& error_message) {
      error_.set(error_message);
      finished_ = true;
    }
    
    std::string WorkerTaskBase::generate_commit_command( const std::vector<std::string>& inputs , const std::vector<std::string>& outputs )
    {
      
      // Get the commit command for this stream operation
      CommitCommand commit_command;
      
      // Input elements
      for (size_t c = 0; c < inputs.size() ; c++) {
        BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", c));
        au::list< BlockRef >::iterator it;
        for (it = block_list->blocks.begin(); it != block_list->blocks.end(); it++) {
          BlockRef *block_ref = *it;
          
          commit_command.RemoveBlock( inputs[c]
                                     , block_ref->block_id()
                                     , block_ref->block_size()
                                     , block_ref->block()->getKVFormat()
                                     , block_ref->range()
                                     , block_ref->info());
        }
      }
      
      // Output elements
      for (size_t c = 0; c < outputs.size(); c++) {
        BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", c));
        au::list< BlockRef >::iterator it;
        for (it = block_list->blocks.begin(); it != block_list->blocks.end(); it++) {
          BlockRef *block_ref = *it;
          
          commit_command.AddBlock( outputs[c]
                                  , block_ref->block_id()
                                  , block_ref->block_size()
                                  , block_ref->block()->getKVFormat()
                                  , block_ref->range()
                                  , block_ref->info());
        }
      }
      
      // Gerate the commit command
      return commit_command.GetCommitCommand();
    }
    
    
  }
}
