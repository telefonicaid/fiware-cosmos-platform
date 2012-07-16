#ifndef _H_WorkerDataElement
#define _H_WorkerDataElement

#include <QtGui>
#include <sstream>

#include "au/RRT.h"
#include "au/string.h"

#include "Plot.h"

#include "UpdateElements.h"  // delilaQTElement

class WorkerDataElement
{
    
public:
    
    std::string name;
    std::string title;
    std::string path;
    
    std::vector<std::string> worker_values;      // Values for each worker
    
    WorkerDataElement( std::string _name , std::string _title , std::string _path );
    std::string pathForWorker( int w );
    void update( au::tables::TreeItem * treeItem );
    
    size_t getUInt64Sum();
    std::string getValueForWorker( size_t w );
    
};



class WorkerDataController
{
    QStandardItemModel model;                      // Data Model for the worker table
    std::vector< WorkerDataElement* > elements;    // All elements controller here
    
public:
    
    void add( std::string name , std::string title , std::string path );
        
    QStandardItemModel* getModel();
    
    void update( au::tables::TreeItem * treeItem );    
    size_t getUInt64Sum( std::string name );
    
    WorkerDataElement* getWorkerDataElement( std::string name );
    
    void reset();
    
};


#endif
