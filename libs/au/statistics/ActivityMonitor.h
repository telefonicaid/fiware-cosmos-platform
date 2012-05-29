

#ifndef _H_AU_ACTIVITY_MONITOR
#define _H_AU_ACTIVITY_MONITOR

#include "au/Object.h"
#include "au/Cronometer.h"

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/tables/Table.h"
#include "au/containers/map.h"

namespace au {
    namespace statistics {
        
        class ActivityItem : public au::Object
        {
            
        public:
            
            std::string name_;   // Name of the item
            double time_;        // Time to be executed
            
            ActivityItem( std::string name , double time )
            {
                name_ = name;
                time_ = time;
            }
        };
        
        
        class ActivityStatistics
        {
            
        public:
            
            double total;        // Total time
            double total_square; // Total time*time
            
            double last;         // Time of the last sample
            
            double min;          // Min time
            double max;          // Max time
            
            size_t counter;      // Number of times
            
            ActivityStatistics();

            void push( ActivityItem* item );

            
            double getAverage();
            double getStdDeviation();
            
        };
        
        class ActivityMonitor
        {
            
            au::Cronometer cronometer_;                // Global cronometer
            std::string current_activty_;             // String describing the current activity  
            double current_activirty_start_time_;             // Check point when the current task started
            
            au::list<ActivityItem> items_; // Last items
            au::map<std::string  , ActivityStatistics > elements_;
            
            au::Token token;
            
        public:
            
            ActivityMonitor( std::string first_activity_name = "init_activity" );            

            void start_activity( std::string activity_name );
            void stop_activity( );
            
            std::string str_last_items();
            std::string str_elements();
            
        private:
            
            void push( ActivityItem* activity_item );

        };
    }
}

#endif
