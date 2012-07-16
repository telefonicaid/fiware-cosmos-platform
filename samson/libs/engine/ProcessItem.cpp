
#include "au/ErrorManager.h"                  // au::ErrorManager
#include "au/xml.h"                         // au::xml...
#include "au/mutex/TokenTaker.h"                          // au::TokenTake
#include "au/ThreadManager.h"

#include "engine/Engine.h"					// engine::Engine
#include "engine/ProcessManager.h"          // engine::ProcessManager
#include "engine/ProcessItem.h"				// Own interface

NAMESPACE_BEGIN(engine)


#pragma mark Background call

void* runProcessItem( void* p )
{
    // Free resources automatically when this thread finish
    pthread_detach(pthread_self());
    
    ProcessItem* processItem = (ProcessItem*) p;
    
    processItem->cronometer.start();
    
    // Run the process
    processItem->run();
    
    processItem->cronometer.stop();
    
    // Notify the process manager that we have finished
    processItem->processManager->finishProcessItem( processItem );
    
    return NULL;
}

#pragma mark ----

ProcessItem::ProcessItem(  int _priority ) :token( "ProcessItem" )
{
    
    priority = _priority;
    progress = 0;		// Initial progress to "0"
    state = queued;
    
    canceled = false;   // By default it is not canceled
    
    operation_name = "unknown";     // By default no statistcs are done
    working_size = 0;               // By default no content to be processed
    
}

ProcessItem::~ProcessItem()
{

}

void ProcessItem::getInfo( std::ostringstream& output)
{
    output << "<process_item>\n";
    
    au::xml_simple( output , "time"  , cronometer.str() ); 
    
    output << "<progress>" << progress << "</progress>\n";
    
    
    output << "<state>";
    switch (state) {
        case queued:
            output << "queued";
            break;
        case running:
            output << "running";
            break;
    }
    output << "</state>\n";
    
    // Inform about priority
    output << "<priority>"<<priority<<"</priority>\n";
    
    // Inform about operation name
    output << "<operation_name>" << operation_name << "</operation_name>\n";
    
    // Inform about sub status for this operation
    output << "<sub_status>" << sub_status << "</sub_status>\n";
    
    
    output << "</process_item>\n";
}

std::string ProcessItem::getStatus()
{
    int p = progress*100.0;
    std::ostringstream o;
    
    switch (state) {
        case queued:
            o << "Q";
            break;
        case running:
            o << "R";
            break;
    }
    
    o << ":" << priority;
    
    o << std::string(":") << operation_name;
    
    if ( sub_status.length() > 0)
        o << ":" << sub_status ;
    
    if ( (p> 0) && (p < 100))
        o << " (" << p << "%)";
    return o.str();
    
}


std::string ProcessItem::getDescription()
{
    return au::str("Process Item '%s' Status: %s"
                   ,operation_name.c_str()
                   ,getStatus().c_str()
                   );
}

void ProcessItem::runInBackground()
{
    au::ExecesiveTimeAlarm alarm("ProcessItem::runInBackground");
    au::ThreadManager::shared()->addThread("ProcessItem::runInBackground", &t, NULL, runProcessItem, this);
}

void ProcessItem::setCanceled()
{
    canceled = true;    
}

bool ProcessItem::isProcessItemCanceled()
{
    return canceled;
}

void ProcessItem::addListenerId( size_t _listenerId )
{
    listeners.insert( _listenerId );
}

void ProcessItem::setProcessItemOperationName( std::string _operation_name )
{
    operation_name = _operation_name;
}

void ProcessItem::setProcessItemWorkingSize( size_t size )
{
    working_size = size;
}        

bool ProcessItem::isRunning()
{
    return (state == running);
}

NAMESPACE_END
