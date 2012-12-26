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
#include <QtGui>


// Include header files for application components.
// ...



#include "tables/Tree.h"

#include "samson/delilah/Delilah.h"
#include "DelilahConnection.h"          // DelilahConnection
#include "StreamOperationWidget.h"                // Own interface

extern DelilahConnection* delilahConnection;

StreamOperationWidget::StreamOperationWidget( QWidget *parent ) : QWidget(parent)
{
    //Setup the 
    setupUi(this);
    
    // Connect change button to change queue to monitor
    connect(button_change, SIGNAL(clicked()), this, SLOT(change_queue()));
    
    // All evolution plot
    plot_size       = new Plot( plot_size_group );
    plot_kvs        = new Plot( plot_kvs_group );
    plot_num_operations = new Plot( plot_num_operations_group );

    // Model for the worker table
    table_workers->setModel( worker_data_controller.getModel() );
    table_workers->resizeColumnsToContents();
    
    // Setup with a particular queue name
    setup();
    
}

void StreamOperationWidget::setup()
{
    worker_data_controller.reset();
    
    // Setup all parameters we are interested at workers
    
    worker_data_controller.add( 
    "kvs" , "#kvs" , au::str("stream_manager/stream_operations/stream_operation[name=%s]/input/block_info/kv_info/kvs", queue_name.c_str() ));
    
    worker_data_controller.add( 
    "size" , "size" , au::str("stream_manager/stream_operations/stream_operation[name=%s]/input/block_info/kv_info/size", queue_name.c_str() ));

    worker_data_controller.add( 
    "num_operations" , "#Operations" , au::str("stream_manager/stream_operations/stream_operation[name=%s]/running_tasks", queue_name.c_str() ));

    // History fields
    
    worker_data_controller.add( 
    "history_kvs" , "history_#kvs" , 
    au::str("stream_manager/stream_operations/stream_operation[name=%s]/history/kv_info/kvs", queue_name.c_str() ));
    
    worker_data_controller.add( 
    "history_size" , "history_size" , 
    au::str("stream_manager/stream_operations/stream_operation[name=%s]/history/kv_info/size", queue_name.c_str() ));
    
    worker_data_controller.add( 
    "history_num_operations" , "history_#Operations" , 
    au::str("stream_manager/stream_operations/stream_operation[name=%s]/history_running_tasks", queue_name.c_str() ));
    
    
    
}

void StreamOperationWidget::update( au::tables::TreeItem * treeItem , bool complete_update )
{    
    
    // Set title
    label_title->setText( au::str( "Operation %s" , queue_name.c_str() ).c_str() );
    
    // Update worker data model
    worker_data_controller.update( treeItem );

    // Adjust columns to content
    table_workers->resizeColumnsToContents();

    
    // Set labels
    label_kvs->setText( au::str(  worker_data_controller.getUInt64Sum("kvs") ).c_str() );
    label_size->setText( au::str(  worker_data_controller.getUInt64Sum("size") ).c_str() );
    label_num_operations->setText( au::str(  worker_data_controller.getUInt64Sum("num_operations") ).c_str() );

    label_history_kvs->setText( au::str(  worker_data_controller.getUInt64Sum("history_kvs") ).c_str() );
    label_history_size->setText( au::str(  worker_data_controller.getUInt64Sum("history_size") ).c_str() );
    label_history_num_operations->setText( au::str(  worker_data_controller.getUInt64Sum("history_num_operations") ).c_str() );
    
    // Samples
    plot_size->push( worker_data_controller.getUInt64Sum("size") , complete_update );
    plot_kvs->push( worker_data_controller.getUInt64Sum("kvs") , complete_update );
    plot_num_operations->push( worker_data_controller.getUInt64Sum("num_operations") , complete_update );

}

void StreamOperationWidget::change_queue()
{
    
    // Take the name of the queue
    queue_name =  entry_queue_name->text().toStdString();
    
    // Setup with the name of the new queue
    setup();
    
}

