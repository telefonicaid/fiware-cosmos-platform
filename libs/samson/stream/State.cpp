

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup


#include "StateItem.h"              // samson::stream::StateItem

#include "BlockList.h"           // samson::stream::BlockList

#include "State.h"  // OwnInterface

namespace samson {
    namespace stream
    {
        
        State::State( std::string _name )
        {
            name = _name;
            
            // Create initial items ( double the number of cores )
            int  num_initial_items = 2*samson::SamsonSetup::getInt("general.num_processess");
            
            if ( num_initial_items <= 0)
                num_initial_items = 1;

            int num_hash_groups_per_item = (KVFILE_NUM_HASHGROUPS) / num_initial_items;
            
            for ( int i = 0 ; i < ( num_initial_items - 1 ) ; i++)
            {                    
                StateItem* item = new StateItem( this  , i*num_hash_groups_per_item , (i+1)*num_hash_groups_per_item );
                items.push_back( item );
            }
            
            // The last one, overs the rest
            StateItem* item = new StateItem( this  , (num_initial_items-1)*num_hash_groups_per_item , KVFILE_NUM_HASHGROUPS );
            items.push_back( item );
         
            paused = false;
            
        }
        
        State::~State()
        {
            // Remove all the item in the state
            items.clearList();
        }
        
        void State::push( BlockList *list )
        {
            
            au::list< StateItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
                (*item)->push( list );

        }
        
        void State::getInfo( std::ostringstream& output)
        {
            output << "<state>\n";
            output << "<name>" << name << "</name>\n";
            
            
            au::list< StateItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
                (*item)->getInfo( output );
            
            output << "</state>\n";
        }
        
        void State::divide( StateItem *item , StateItem *item1 , StateItem *item2 )
        {
            // Divide this item in two items
            au::list< StateItem >::iterator item_it , item_it1 , item_it2;
            
            for (item_it = items.begin() ; item_it != items.end() ; item_it++)
            {
                if( (*item_it) == item)
                {
                    
                    item_it1 = items.erase( item_it );
                    
                    item_it2 = items.insert(item_it1, item1);
                    items.insert(item_it2, item1);
                    
                    // Delete the old item
                    delete item;
                    
                    return;
                    
                }
            }

            
        }
        
        std::string State::getStatus()
        {
            
            std::ostringstream output;
            
            output << "State " << name;

            output << " ( " << items.size() << " items containing " << getFullKVInfo().str() << " ) ";
            
            if( paused )
                output << " [ PAUSED ]";
                
            output << "\n";            
            output << "\n";            
            
            au::list< StateItem >::iterator item;
            for ( item = items.begin() ; item != items.end() ; item++ )
            {
                StateItem *stateItem = *item;
                output << "\tItem [ " << stateItem->hg_begin << " " << stateItem->hg_end << " ]\n";
                output << "\t  Input:\n";
                output << au::indent( stateItem->list->str() ) << "\n";
                output << "\t  State:\n";
                output << au::indent( stateItem->state->str() ) << "\n";
                
            }
            return output.str();
        }
        
        bool State::isWorking()
        {
            au::list< StateItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
                if( (*item)->isWorking() )
                    return true;
            
            return false;
            
        }
        
        FullKVInfo State::getFullKVInfo()
        {
            FullKVInfo info;
            au::list< StateItem >::iterator item;
            for ( item = items.begin() ; item != items.end() ; item++ )
            {
                StateItem *StateItem = *item;
                info.append( StateItem->state->getFullKVInfo() );
            }
            
            return info;
            
            
        }
    }
}