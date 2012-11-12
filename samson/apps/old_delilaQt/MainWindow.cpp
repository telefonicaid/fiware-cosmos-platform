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

#include "tables/Tree.h"        // au::tables::TreeItem

// SAMSON Stuff
#include "samson/delilah/Delilah.h"

// Widgets
#include "SamsonConnect.h"
#include "SamsonQueryWidget.h"
#include "SamsonNodeWidget.h"
#include "PlotWidget.h"
#include "QueueWidget.h"
#include "StreamOperationWidget.h"

#include "DelilahConnection.h"     // DelilahConnection

#include "UpdateElements.h"

#include "MainWindow.h" // Own interface

extern SamsonQueryWidget* samsonQueryWidget;
extern DelilahConnection * delilahConnection;
extern SamsonConnect * samsonConnect;

MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent )
{
    // Init flag
    flag_init = false;
    
    //Setup the 
    setupUi(this);
    
    // Connections
    connect(samsonQueryPushButton, SIGNAL(clicked()), this, SLOT(open_samson_query()));
    //connect( treeUpdateButton , SIGNAL(clicked()) , this , SLOT( updateTree() ) );
    connect( queuesButton , SIGNAL(clicked()) , this , SLOT( show_queues() ) );
	connect( streamOperationsButton , SIGNAL( clicked()), this , SLOT( show_stream_operations() ));
    
    // General update model
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(2000);
    
}

void MainWindow::updateTimer()
{
    
    if( delilahConnection->isReady() )
    {
        if( !flag_init )
        {
            init(); // Init this widget ( only once )
            show(); // Show this widget
                        
            samsonConnect->hide();
            flag_init = true;
        }				
    }
	
	// Update all elements accumulated in updateElements
	updateDelilagQTElements( );

}

// Update information in the main tree
void MainWindow::updateTree()
{
    if( delilahConnection->isReady() )
    {
        // Get my own copy of all tree item
        //au::tables::TreeItem* treeItem =  delilahConnection->delilah->getTreeItem();
        //setModel( itemModelFromTreeItem( treeItem ) );
        //delete treeItem;
    }
}


void MainWindow::update( )
{
    // Update information from delilah...

    /*
    for (size_t i = 0 ; i < workerSamsonNodeWidgets.size() ; i++ )
    {
        au::tables::TreeItem* worker_tree = treeItem->getFirstTreeFromPath( au::str("worker[%d]" , (int) i ) );
        if( worker_tree )
        {
            workerSamsonNodeWidgets[i]->update( worker_tree );
            delete worker_tree;
        }
    }
     */
}

void MainWindow::init()
{
    num_workers = delilahConnection->delilah->network->getNumWorkers();
    for (int i = 0 ; i < num_workers ; i++ ) 
    {
        SamsonNodeWidget *node = new SamsonNodeWidget( );
        workerSamsonNodeWidgets.push_back( node );

        node->setTitle( au::str( "Worker %d" , i).c_str());
        verticalLayout->addWidget(node);
    }    
}

void MainWindow::open_samson_query()
{
    samsonQueryWidget->show();    
}

void MainWindow::show_queues()
{
   // New Queue Widget
   QueueWidget * widget = new QueueWidget();
   widget->show();
}

void MainWindow::show_stream_operations()
{
   // New Queue Widget
   StreamOperationWidget * widget = new StreamOperationWidget();
   widget->show();
}


