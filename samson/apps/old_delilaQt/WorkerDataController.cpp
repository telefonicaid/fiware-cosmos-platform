/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */


#include "WorkerDataController.h"   // Own interface


WorkerDataElement::WorkerDataElement( std::string _name , std::string _title , std::string _path )
{
    name = _name;
    title = _title;
    path = _path;
    
}

std::string WorkerDataElement::pathForWorker( int w )
{
    return au::str("worker[%d]/%s", w , path.c_str() );
}

//std::string key_path    = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/format/key_format", w , queue_name.c_str() );

void WorkerDataElement::update( au::tables::TreeItem * treeItem )
{    
    // Update content from item
    int num_workers = (int) treeItem->getNumItems("worker");
    
    worker_values.clear();
    for (int w = 0 ; w < num_workers ; w++ )
        worker_values.push_back( treeItem->get( pathForWorker(w) ) );
}

size_t WorkerDataElement::getUInt64Sum()
{
    size_t total = 0;
    for (size_t w = 0 ; w < worker_values.size() ; w++ )
        total += atoll( worker_values[w].c_str() );
    return total;
}


std::string WorkerDataElement::getValueForWorker( size_t w )
{
    if( w >= worker_values.size() )
        return "";
    return worker_values[w];
}

#pragma mark WorkerDataController

void WorkerDataController::add( std::string name , std::string title , std::string path )
{
    elements.push_back( new WorkerDataElement( name , title , path ) );
}

QStandardItemModel* WorkerDataController::getModel()
{
    return &model;
}

void WorkerDataController::update( au::tables::TreeItem * treeItem )
{
    LM_M(("Update %lu elements" , elements.size()));
    
    // Get number of workers
    int num_workers = (int) treeItem->getNumItems("worker");
    
    // Update all elements
    for( size_t i = 0 ; i < elements.size() ; i++)
        elements[i]->update( treeItem );
    
    // Update model
    // ------------------------------------------------------------------
    
    model.clear();
    for( size_t i = 0 ; i < elements.size() ; i++)
        model.setHorizontalHeaderItem( i , new QStandardItem( QString(  elements[i]->title.c_str() ) ) );
    
    for (int w = 0 ; w < num_workers ; w++ ) 
        for( size_t i = 0 ; i < elements.size() ; i++)
            model.setItem(w, i, new QStandardItem( QString( elements[i]->getValueForWorker( w ).c_str() ) ) );
    // ------------------------------------------------------------------        
}

size_t WorkerDataController::getUInt64Sum( std::string name )
{
    WorkerDataElement *element = getWorkerDataElement( name );
    
    if( !element )
        return 0;
    else
        return element->getUInt64Sum();
}

WorkerDataElement* WorkerDataController::getWorkerDataElement( std::string name )
{
    for( size_t i = 0 ; i < elements.size() ; i++)
        if( elements[i]->name == name )
            return elements[i];
    return  NULL;
}

void WorkerDataController::reset()
{
    for( size_t i = 0 ; i < elements.size() ; i++)
        delete elements[i];
    
    elements.clear();
}