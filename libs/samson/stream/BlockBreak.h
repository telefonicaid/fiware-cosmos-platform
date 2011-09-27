
#ifndef _H_STREAM_BLOCK_BREAK
#define _H_STREAM_BLOCK_BREAK

#include <string>
#include <sstream>

/*
 
namespace samson {
    
    class SamsonWorker;
    class Info;
    
    namespace stream
    {
        
        class BlockList;

        
        class BlockBreakItem
        {
            size_t id;                      // Identifier of the original block
            int num_divisions;              // Number of range divisions    
            BlockList *list;                // BLock list with the decomposition of this block
            bool ready;
            
            friend class StreamManager;                     // For state-save to disk
            
        public:
            
            BlockBreakItem( size_t _id ,  int _num_divisions);
            ~BlockBreakItem();
            
            void update( BlockList *_list );

            // Get xml information
            void getInfo( std::ostringstream &output);

            // Check if ready
            bool isReady();
            
            // Get list
            BlockList* getBlockList();
            
        };
        
        class BlockBreak
        {
            
            size_t id;                                      // Id of the block we are decomposing here
            au::map< int , BlockBreakItem > items;          // Collection of possible divisions for this block

            friend class StreamManager;                     // For state-save to disk
            
        public:
            
            BlockBreak( size_t _id );
            ~BlockBreak();
            
            // Get xml information
            void getInfo( std::ostringstream &output);
          
            // Get max fivision ( computed or planned )
            int getMaxDivision();
            
            // Add a particular division
            void addNumDivisions( int num_divisions );
            
            // Void update a particular division
            void update( int num_divisions , BlockList *_list );
            
            // Get the maximum division currently ready
            int getMaxDivisionReady();
            
            // Get the block list for the maximum division
            BlockList *getMaxDivisionBlockList();
            
            // Remove inferior divisions
            void removeSmallerDivisions();
        private:
            
            BlockBreakItem* getItem( int num_division );
        };
	}
}
*/
#endif

