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
#include "QueueWidget.h"                // Own interface

extern DelilahConnection* delilahConnection;

QueueWidget::QueueWidget( QWidget *parent ) : QWidget(parent)
{
    //Setup the 
    setupUi(this);
    
    // Connect change button to change queue to monitor
    connect(button_change, SIGNAL(clicked()), this, SLOT(change_queue()));
    
    // All evolution plot
    plot_size       = new Plot( plot_size_group );
    plot_kvs        = new Plot( plot_kvs_group );
    plot_num_blocks = new Plot( plot_num_blocks_group );
    plot_rate_size  = new Plot( plot_rate_size_group );
    plot_rate_kvs   = new Plot( plot_rate_kvs_group );

    // Model for the worker table
    table_workers->setModel( worker_data_controller.getModel() );
    table_workers->resizeColumnsToContents();
    
    // Setup with a particular queue name
    setup();
    
    // Plot disk_memory
    {
        KDChart::BarDiagram* diagram = new KDChart::BarDiagram;
        diagram->setModel(&m_model_disk_memory);
        
        m_chart_disk_memory.coordinatePlane()->replaceDiagram(diagram);
        QVBoxLayout* l = new QVBoxLayout(this);
        l->addWidget(&m_chart_disk_memory);
        disk_memory_plot_group->setLayout(l);
        
        // Horizontal staked element
        diagram->setType( KDChart::BarDiagram::Stacked );
        diagram->setOrientation( Qt::Horizontal );
    }
    
}

void QueueWidget::setup()
{
    worker_data_controller.reset();
    
    // Setup all parameters we are interested at workers
    worker_data_controller.add( "key_format" , "key_format" ,  au::str("stream_manager/queues/queue[name=%s]/format/key_format", queue_name.c_str() ));
    worker_data_controller.add( "value_format" , "value_format" , au::str("stream_manager/queues/queue[name=%s]/format/value_format", queue_name.c_str() ));
    
    worker_data_controller.add( "kvs" , "#kvs" , au::str("stream_manager/queues/queue[name=%s]/block_info/kv_info/kvs", queue_name.c_str() ));
    worker_data_controller.add( "size" , "size" , au::str("stream_manager/queues/queue[name=%s]/block_info/kv_info/size", queue_name.c_str() ) );
    
    worker_data_controller.add( "num_blocks" , "#blocks" , au::str("stream_manager/queues/queue[name=%s]/block_info/num_blocks", queue_name.c_str() ) );
    
    worker_data_controller.add( "rate_size" , "Bytes/second" , au::str("stream_manager/queues/queue[name=%s]/rate_size/rate", queue_name.c_str() ));
    worker_data_controller.add( "rate_kvs" , "kvs/second   " , au::str("stream_manager/queues/queue[name=%s]/rate_kvs/rate",  queue_name.c_str() ));

    worker_data_controller.add( "total_size" ,
                               "bytes" , au::str("stream_manager/queues/queue[name=%s]/block_info/size", queue_name.c_str() ));
    
    worker_data_controller.add( "total_size_on_memory" , 
                               "on_memory" , au::str("stream_manager/queues/queue[name=%s]/block_info/size_on_memory",  queue_name.c_str() ));
    
    worker_data_controller.add( "total_size_on_disk" , 
                               "on_disk" , au::str("stream_manager/queues/queue[name=%s]/block_info/size_on_disk", queue_name.c_str() ));
    
}

void QueueWidget::update( au::tables::TreeItem * treeItem , bool complete_update )
{    
    
    // Set title
    label_title->setText( au::str( "Queue %s" , queue_name.c_str() ).c_str() );
    
    // Update worker data model
    worker_data_controller.update( treeItem );

    // Adjust columns to content
    table_workers->resizeColumnsToContents();

    
    // Set labels
    label_kvs->setText( au::str(  worker_data_controller.getUInt64Sum("kvs") ).c_str() );
    label_size->setText( au::str(  worker_data_controller.getUInt64Sum("size") ).c_str() );
    label_num_blocks->setText( au::str(  worker_data_controller.getUInt64Sum("num_blocks") ).c_str() );
    label_rate_kvs->setText( au::str(  worker_data_controller.getUInt64Sum("rate_kvs") ).c_str() );
    label_rate_size->setText( au::str(  worker_data_controller.getUInt64Sum("rate_size") ).c_str() );
    
    // Samples
    plot_size->push( worker_data_controller.getUInt64Sum("size") , complete_update );
    plot_kvs->push( worker_data_controller.getUInt64Sum("kvs") , complete_update );
    plot_num_blocks->push( worker_data_controller.getUInt64Sum("num_blocks") , complete_update );
    plot_rate_size->push( worker_data_controller.getUInt64Sum("rate_size") , complete_update );
    plot_rate_kvs->push( worker_data_controller.getUInt64Sum("rate_kvs") , complete_update );
    
    // Update evolution disk_memory
    m_model_disk_memory.clear();
    size_t total_size_total =  worker_data_controller.getUInt64Sum("total_size");
    size_t total_size_on_memory =  worker_data_controller.getUInt64Sum("total_size_on_memory");
    size_t total_size_on_disk =  worker_data_controller.getUInt64Sum("total_size_on_disk");
    
    m_model_disk_memory.setItem(0, 0, new QStandardItem( QString( au::str( "%lu", total_size_total - total_size_on_disk  ).c_str() ) ) );
    m_model_disk_memory.setItem(0, 1, new QStandardItem( QString( au::str( "%lu", total_size_on_memory + total_size_on_memory - total_size_total ).c_str() ) ) );
    m_model_disk_memory.setItem(0, 2, new QStandardItem( QString( au::str( "%lu", total_size_total - total_size_on_memory ).c_str() ) ) );
    
}


void QueueWidget::change_queue()
{
    
    // Take the name of the queue
    queue_name =  entry_queue_name->text().toStdString();
    
    // Setup with the name of the new queue
    setup();
    
}

