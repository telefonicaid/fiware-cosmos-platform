

#include "samson/common/coding.h"   // KVFILE_NUM_HASHGROUPS

#include "StateItem.h"              // samson::stream::StateItem

#include "BlockList.h"           // samson::stream::BlockList

#include "State.h"  // OwnInterface

namespace samson {
    namespace stream
    {
        
        State::State( std::string _name )
        {
            name = _name;
            
            StateItem* mainItem = new StateItem( this  , 0 , KVFILE_NUM_HASHGROUPS );
            items.push_back( mainItem );
            
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
            
            for (item_it = items.begin() ; item_it != items.end() ; item++)
            {
                if( (*item_it) == item)
                {
                    item_it1 = items.erase( item_it );
                    
                    item_it2 = items.insert(item_it1, item1);
                    items.insert(item_it2, item1);
                }
            }

            // Delete the old item
            delete item;
            
        }
        
        std::string State::getStatus()
        {
            
            std::ostringstream output;
            
            output << "State " << name << "\n";            
            
            au::list< StateItem >::iterator item;
            for ( item = items.begin() ; item != items.end() ; item++ )
            {
                StateItem *stateItem = *item;
                output << "\tItem [ " << stateItem->hg_begin << " " << stateItem->hg_end << " ]\n";
                output << au::Format::indent( stateItem->state->str() ) << "\n";
                
            }
            return output.str();
        }
        
        
    }
}