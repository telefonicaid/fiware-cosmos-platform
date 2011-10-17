#include <QtGui>


// Include header files for application components.
// ...

#include "au/Tree.h"        // au::TreeItem

// SAMSON Stuff
#include "samson/delilah/Delilah.h"

// Widgets
#include "SamsonConnect.h"
#include "SamsonQueryWidget.h"
#include "SamsonNodeWidget.h"
#include "PlotWidget.h"

#include "DelilahConnection.h"     // DelilahConnection


#include "MainWindow.h" // Own interface

extern SamsonQueryWidget* samsonQueryWidget;
extern DelilahConnection * delilahConnection;
extern SamsonConnect * samsonConnect;

MainWindow::MainWindow( QWidget *parent ) : QMainWindow(parent)
{
    // Init flag
    flag_init = false;
    
    //Setup the 
    setupUi(this);
    
    // Connections
    connect(samsonQueryPushButton, SIGNAL(clicked()), this, SLOT(open_samson_query()));
    connect( treeUpdateButton , SIGNAL(clicked()) , this , SLOT( updateTree() ) );
    connect( queuesButton , SIGNAL(clicked()) , this , SLOT( show_queues() ) );

    // Init my model
    myModel = NULL;
    
    controllerSamsonNodeWidget = NULL;
    delilahSamsonNodeWidget = NULL;
    
    
    // General update model
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(1000);
    
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
        
        // Get a copy of all tree item
        au::TreeItem* treeItem =  delilahConnection->delilah->getTreeItem( );
        update( treeItem ); // Update contents of this widget
        delete treeItem;
    }
    
}

void MainWindow::updateTree()
{
    if( delilahConnection->isReady() )
    {
        // Get my own copy of all tree item
        au::TreeItem* treeItem =  delilahConnection->delilah->getTreeItem( );
        setModel( itemModelFromTreeItem( treeItem ) );
        delete treeItem;
    }
}

void MainWindow::update( au::TreeItem *treeItem )
{
    delilahSamsonNodeWidget->update( treeItem->getItemFromPath("delilah") );
    controllerSamsonNodeWidget->update( treeItem->getItemFromPath("controller") );
    
    for (size_t i = 0 ; i < workerSamsonNodeWidgets.size() ; i++ )
        workerSamsonNodeWidgets[i]->update( treeItem->getItemFromPath( au::str("worker[%d]" , (int) i ) ) );
}


void MainWindow::setModel( QStandardItemModel *_myModel )
{
    // Model of the tree
    treeView->setModel( _myModel );
    
    // Remove the previous one...
    if( myModel )
        delete myModel;
    
    myModel = _myModel;
}

void MainWindow::init()
{
    // Create a widget for every node
    controllerSamsonNodeWidget = new SamsonNodeWidget( );
    verticalLayout->addWidget(controllerSamsonNodeWidget);
    controllerSamsonNodeWidget->setTitle( "Controller " );

    num_workers = delilahConnection->delilah->network->getNumWorkers();
    for (int i = 0 ; i < num_workers ; i++ ) 
    {
        SamsonNodeWidget *node = new SamsonNodeWidget( );
        workerSamsonNodeWidgets.push_back( node );

        node->setTitle( au::str( "Worker %d" , i).c_str());
        verticalLayout->addWidget(node);
    }
    
    // Create a widget for every node
    delilahSamsonNodeWidget = new SamsonNodeWidget( );
    verticalLayout->addWidget(delilahSamsonNodeWidget);
    delilahSamsonNodeWidget->setTitle( "Delilah " );
    
    
}



QStandardItemModel* MainWindow::itemModelFromTreeItem( au::TreeItem* treeItem )
{
    QStandardItemModel *_myModel = new QStandardItemModel();
    _myModel->appendRow( itemFromTreeItem( treeItem ) );
    return _myModel;
}

QStandardItem* MainWindow::itemFromTreeItem( au::TreeItem* treeItem )
{
    std::string value = treeItem->getValue();
    
    QStandardItem *itemModel = new QStandardItem( QString( value.c_str() ) );
    
    for (size_t i = 0; i < treeItem->getNumItems() ; ++i) 
    {
        QStandardItem* _itemModel = itemFromTreeItem( treeItem->getItem( i ) );
        itemModel->appendRow( _itemModel );
    } 
    
    return itemModel;
}


void MainWindow::open_samson_query()
{
    samsonQueryWidget->show();    
}


void MainWindow::show_queues()
{
  PlotWidget *plotWidget = new PlotWidget();
  plotWidget->show();
}


