

#include "ActivityMonitor.h" // Own interface
#include <math.h>

namespace au {
    namespace statistics {
        
        ActivityStatistics::ActivityStatistics()
        {
            total = 0;
            total_square = 0;
            last= 0;
            min = 0;
            max = 0;
            counter = 0;
        }
        
        void ActivityStatistics::push( ActivityItem* item )
        {
            // Get the time of this item
            double t = item->time_;
            
            if( t < 0 )
            {
                LM_W(("Reported negative time (%d) for item %s" , t ,  item->name_.c_str() ));
                return;
            }
            
            counter++; // Update the counter ( # of times executed )
            total += t;
            total_square += t*t;
            last = t;
            
            if( counter == 1 )
            {
                min = t;
                max = t;
            }
            else
            {
                // Update min & max
                if( t < min )
                    min = t;
                if( t > max )
                    max = t;
            }
            
        }
        
        double ActivityStatistics::getAverage()
        {
            if ( counter == 0 )
                return 0;
            return total / (double)counter;
        }
        
        double ActivityStatistics::getStdDeviation()
        {
            if ( counter == 0 )
                return 0;
            
            double tmp = ( total_square / (double)counter ) - getAverage()*getAverage();
            if( tmp < 0 )
                return 0;
            return sqrt( tmp );
        }
        
        
        
        ActivityMonitor::ActivityMonitor( std::string first_activity_name ) : token("EngineStatistics")
        {
            current_activty_ = first_activity_name;
            current_activirty_start_time_ = 0;
        }
        
        void ActivityMonitor::start_activity( std::string activity_name )
        {
            au::TokenTaker tt(&token);
            double stop_time = cronometer_.diffTime();
            double time = stop_time - current_activirty_start_time_;
            
            push( new ActivityItem( current_activty_ , time ) );
            
            // Change the name of the current activty
            current_activirty_start_time_ = stop_time;
            current_activty_ = activity_name;
            
        }
        
        void ActivityMonitor::stop_activity( )
        {
            // Mutex protection inside start_activity
            start_activity( "no_activity" );
        }
        
        std::string ActivityMonitor::str_last_items()
        {
            au::TokenTaker tt(&token);
            
            au::tables::Table table( "Item|Time,left,f=double" );
            au::list<ActivityItem>::iterator it;
            for ( it = items_.begin() ; it != items_.end() ; it++ )
                table.addRow( au::StringVector( (*it)->name_ , au::str( "%.12f" , (*it)->time_ ).c_str() ) );
            return table.str();
        }
        
        std::string ActivityMonitor::str_elements()
        {
            au::TokenTaker tt(&token);
            
            au::tables::Table table( "Element|Num,f=uint64|Total time,f=double|Average,f=double|std dev,f=double|Min,f=double|Max,f=double" );
            au::map<std::string  , ActivityStatistics >::iterator it;
            for ( it = elements_.begin() ; it != elements_.end() ; it++ )
            {
                au::StringVector values;
                
                values.push_back( it->first );
                
                values.push_back( au::str( "%lu", (size_t) (it->second->counter )) );
                values.push_back( au::str( "%.12f", it->second->total ));
                                
                values.push_back( au::str( "%.12f", it->second->getAverage() ));
                values.push_back( au::str( "%.12f", it->second->getStdDeviation() ));
                
                values.push_back( au::str( "%.12f", it->second->min ));
                values.push_back( au::str( "%.12f", it->second->max ));
                
                table.addRow( values );
            }
            
            au::StringVector sort_fields = au::StringVector( "Total time" );
            table.sort( sort_fields );
            
            return table.str();
        }
        
        void ActivityMonitor::push( ActivityItem* activity_item )
        {
            au::TokenTaker tt(&token);
            
            // Inset in the list of last items
            activity_item->retain();
            items_.push_back( activity_item );
            
            // Only keep the list of last 100 elements
            while( items_.size() > 100 )
            {
                ActivityItem* tmp_item = items_.extractFront();
                tmp_item->release();
            }
            
            // Update the associated element statustics
            ActivityStatistics * activity_estatistics = elements_.findOrCreate( activity_item->name_ );
            activity_estatistics->push( activity_item );
        }
        
        std::string ActivityMonitor::getCurrentActivity()
        {
            au::TokenTaker tt(&token);
            
            return current_activty_;
        }

    }
}