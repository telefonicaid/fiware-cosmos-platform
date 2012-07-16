/* ****************************************************************************
 *
 * FILE            ProcessItem
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * ProcessItem defined the task to be executed by ProcessManager
 * It should be an independent task ( no external access while running )
 * It is managed by the process manager
 *
 * ****************************************************************************/

#ifndef _H_PROCESS_ITEM
#define _H_PROCESS_ITEM


#include <string>
#include <sstream>

#include "au/Cronometer.h"              // au::CronometerSystem
#include "au/Environment.h"            // au::Environment
#include "au/mutex/Token.h"                  // au::Token
#include "au/ErrorManager.h"           // au::ErrorManager
#include "au/namespace.h"

#include "au/Object.h"

#define PI_PRIORITY_NORMAL_OPERATION	1
#define PI_PRIORITY_NORMAL_COMPACT		5		// Compact operation
#define PI_PRIORITY_BUFFER_PREWRITE		10		// Processing an output buffer to generate what is stored on disk


/**
 
 ProcessItem
 
 item inside the ProcessManager
 
 An item is a task to do without any IO operation in the middle.
 It only requires processing time
 */

NAMESPACE_BEGIN(engine)


class ProcessItem;
class ProcessManagerDelegate;
class ProcessManager;

class ProcessItem  : public au::Object
{
    
    // Identifiers of the listeners that should be notified when operation is finished
    std::set<size_t> listeners;
    
    friend class ProcessManager;
    
protected:
    
    // Operation name for statistics
    std::string operation_name;
    size_t working_size;
    
public:
    
    typedef enum 
    {
        queued,         // In the queue waiting to be executed
        running,        // Running in a background process
    } ProcessItemStatus;
    
    bool canceled;      // Flag to indicate that this process has been cancelled ( not forced exit )
    
    // Internal state of the process
    ProcessItemStatus  state;
    
    // Pointer to the process manager to notify 'halt' 'finish' and so...
    ProcessManager* processManager;
    
    // Thread used to run this in background
    pthread_t t;
    
public:
    
    //Cronometer to measure time running
    au::CronometerSystem cronometer;
    
private:
    
    // Token used to protect state and block main thread when necessary
    au::Token token;	
    
protected:
    
    // Information about the status of this process
    double progress;					// Progress of the operation ( if internally reported somehow )
    std::string sub_status;				// Letter describing internal status
    
public:
    
    // Error management
    au::ErrorManager error;                     
    
    // Priority level ( 0 = low priority ,  10 = high priority )
    int priority;                               
    
    // Environment variables
    au::Environment environment;
    
    // Constructor with priority
    ProcessItem( int _priority = 5 );
    virtual ~ProcessItem();
    
    // Status management
    std::string getStatus();
    
    // What to do when processor is available
    virtual void run()=0;
    
    // Function to create a new thread and run "run" in background
    void runInBackground();

    // Get description of this process item
    std::string getDescription();
    
protected:
    
    void halt();			// command executed inside run() to stop the task until ready returns true
    
public:
        
    void setCanceled();
    bool isProcessItemCanceled();
    
    bool isRunning();
    
public:
    
    void addListenerId( size_t _listenerId );
    
public:
    
    // Get information for xml monitoring
    void getInfo( std::ostringstream& output);
    
protected:
    
    // Interface to monitor operations performance
    void setProcessItemOperationName( std::string _operation_name );
    void setProcessItemWorkingSize( size_t size );
	
};

NAMESPACE_END

#endif
