
#include <math.h>

#include "OnOffMonitor.h" // Own interface

namespace au 
{
    OnOffMonitor::OnOffMonitor()
    {
        on = false; // By default it is initially false
        on_time = 0;
        off_time = 0;
        
        double time_span = 30; // Last 30 secs
        f = ( time_span - 1 ) / time_span;
        
    }
    
    double OnOffMonitor::get_on_time()
    {
        return on_time;
    }
    
    double OnOffMonitor::get_off_time()
    {
        return off_time;
    }
    
    void OnOffMonitor::set_on( bool _on )
    {
        double t = cronometer.diffTime();
        
        if( on )
            on_time += t;
        else
            off_time += t;
        
        // Forgetting factor
        off_time *= ::pow( f , t ); 
        on_time *= ::pow( f , t ); 
        
        // Reset cronometer
        cronometer.reset();
        
        // Change state if any
        on = _on;
    }
    
    double OnOffMonitor::getActivity()
    {
        double t = cronometer.diffTime();
        double _on_time = on_time;
        double _off_time = off_time;
        if( on )
            _on_time += t;
        else
            _off_time += t;
        
        if( ( on_time + off_time ) == 0 )
            return 0;
        else
            return on_time / ( on_time + off_time );
    }
}