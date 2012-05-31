
#include "au/string.h"
#include "au/mutex/TokenTaker.h"

#include "Rate.h"        // Own interface

namespace au {
    namespace rate {
        
        Rate::Rate( int num_samples ) : token("Rate")
        {
            // Init total counters
            total_size_ = 0;
            total_num_ = 0;
            
            // Init vectors
            num_samples_ = num_samples;
            hits_ = (int*) malloc( num_samples_ * sizeof(int) );
            size_ = (double*) malloc( num_samples_ * sizeof(double) );
            
            for ( int i = 0 ; i < num_samples_ ; i++ )
            {
                hits_[i] = 0;
                size_[i] = 0;
            }

            last_time_correction = 0;
            
        }
        
        Rate::~Rate( )
        {
            free( hits_ );
            free( size_ );
        }
        
        void Rate::push( size_t size )
        {
            au::TokenTaker tt(&token);
            
            _update_time();
            
            // Global counters
            total_size_ += size;
            total_num_++;

            // Update only the first slot    
            hits_[0]++;
            size_[0]+= size;
        }
        
        std::string Rate::str()
        {
            au::TokenTaker tt(&token);
            
            // Update slots if necessary
            _update_time();
            
            //update_values();
            return au::str("[ Currently %s %s ] [ Accumulated in %s %s with %s ]" 
                           , au::str( _getHitRate()  , "hits/s" ).c_str() 
                           , au::str( _getRate() , "B/s" ).c_str() 
                           , au::str_detail( total_num_ , "hits" ).c_str() 
                           , au::str_detail( total_size_ , "B" ).c_str() 
                           );
        }
        
        std::string Rate::strAccumulatedAndRate()
        {
            au::TokenTaker tt(&token);
            
            //update_values();
            return au::str("[ Accumulated %s at %s ]" 
                           , au::str_detail( total_size_ , "B" ).c_str() 
                           , au::str( _getRate() , "B/s" ).c_str() 
                           );
        }
        
        size_t Rate::getTotalNumberOfHits()
        {
            return total_num_;
        }
        size_t Rate::getTotalSize()
        {
            return total_size_;
        }
        
        double Rate::getRate()
        {
            au::TokenTaker tt(&token);
            _update_time();
            return  _getRate();
        }
        
        double Rate::getHitRate()
        {
            au::TokenTaker tt(&token);
            _update_time();
            return  _getHitRate();
        }
        
        double Rate::_getHitRate()
        {
            double total = 0;
            for ( int i = 1 ; i < num_samples_ ; i++ )
                total += hits_[i];
            double tmp = total / (double)(num_samples_-1);

            if( tmp < hits_[0] )
                return hits_[0]; // hits in the last second
            
            return tmp;
            
        }
        
        double Rate::_getRate()
        {
            double total = 0;
            for ( int i = 1 ; i < num_samples_ ; i++ )
                total += size_[i];
            double tmp = total / (double)(num_samples_-1);
            
            if( tmp < size_[0] )
                return size_[0]; // Size in the last second
            
            return tmp;
            
        }
        
        void Rate::_update_time()
        {
            // Take current time
            size_t time = c.diffTimeInSeconds();
            
            if( time > last_time_correction )
            {
                // Compute difference with the last reference
                size_t diff = time - last_time_correction;
                
                if( diff == 0 )
                    return;
                
                // Move samples
                for( int i = 0 ; i < ( (int)num_samples_ - (int)diff ) ; i++ )
                {
                    size_[ num_samples_ - i - 1 ] = size_[ num_samples_ - i - 2 ];
                    hits_[ num_samples_ - i - 1 ] = hits_[ num_samples_ - i - 2 ];
                }
                
                // Init the new slots
                for ( int i = 0 ; ( i < (int) diff ) && ( i < num_samples_ ) ; i++ )
                {
                    size_[i] = 0;
                    hits_[i] = 0;
                }
                
                // Set the new reference
                last_time_correction = time;                
            }
            

        }
 
    }
}
