#include <QtGui>


// Include header files for application components.
// ...



#include "au/Tree.h"

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
    
    // Update every second
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
    
    workers_model = NULL;

    // All evolution plot
    plot_size = new Plot( plot_size_group );
    plot_kvs = new Plot( plot_kvs_group );
    plot_num_blocks = new Plot( plot_num_blocks_group );
    plot_rate_size = new Plot( plot_rate_size_group );
    plot_rate_kvs = new Plot( plot_rate_kvs_group );
    
    
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

void QueueWidget::update()
{
    
    if( delilahConnection->isReady() )
    {
        // Get a copy of all tree item
        au::TreeItem* treeItem =  delilahConnection->delilah->getTreeItem( );
        
        if( !treeItem )
        {
            label_title->setText( "No info!" );
            return;
        }
        
        // Update content from item
        int num_workers = (int) treeItem->getNumItems("worker");
        
        // Set title
        label_title->setText( au::str( "Queue %s" , queue_name.c_str() ).c_str() );

        // Contents for table
        
        QStandardItemModel* _myModel = new QStandardItemModel();
        
        // Fill the model
        _myModel->setHorizontalHeaderItem( 0 , new QStandardItem( QString(  "Worker" ) ) );
        _myModel->setHorizontalHeaderItem( 1 , new QStandardItem( QString(  "Key" ) ) );
        _myModel->setHorizontalHeaderItem( 2 , new QStandardItem( QString(  "Value" ) ) );
        _myModel->setHorizontalHeaderItem( 3 , new QStandardItem( QString(  "kvs" ) ) );
        _myModel->setHorizontalHeaderItem( 4 , new QStandardItem( QString(  "size" ) ) );
        _myModel->setHorizontalHeaderItem( 5 , new QStandardItem( QString(  "#blocks" ) ) );
        _myModel->setHorizontalHeaderItem( 6 , new QStandardItem( QString(  "kvs/s" ) ) );
        _myModel->setHorizontalHeaderItem( 7 , new QStandardItem( QString(  "size/s" ) ) );

        _myModel->setHorizontalHeaderItem( 8 , new QStandardItem( QString(  "total_size" ) ) );
        _myModel->setHorizontalHeaderItem( 9 , new QStandardItem( QString(  "total_on_memory" ) ) );
        _myModel->setHorizontalHeaderItem( 10 , new QStandardItem( QString(  "total_on_disk" ) ) );
        

        // Accumulated version of the infromation
        size_t total_kvs = 0;
        size_t total_size = 0;
        int total_num_blocks =  0;
        size_t total_rate_kvs = 0;
        size_t total_rate_size = 0;
        
        size_t total_size_on_memory = 0;
        size_t total_size_on_disk   = 0;
        size_t total_size_total     = 0;
        
        for (int w = 0 ; w < num_workers ; w++ ) 
        {
            _myModel->setItem(w, 0, new QStandardItem( QString( au::str("%d",w).c_str() ) ) );
            
            std::string key_path    = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/format/key_format", w , queue_name.c_str() );
            std::string value_path  = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/format/value_format", w , queue_name.c_str() );
            std::string size_path   = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/block_info/kv_info/size", w , queue_name.c_str() );
            std::string kvs_path    = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/block_info/kv_info/kvs", w , queue_name.c_str() );
            std::string num_blocks_path  = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/block_info/num_blocks", w , queue_name.c_str() );
            std::string rate_size_path   = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/rate_size/rate", w , queue_name.c_str() );
            std::string rate_kvs_path    = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/rate_kvs/rate", w , queue_name.c_str() );

            std::string size_on_memory_path  = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/block_info/size_on_memory", w , queue_name.c_str() );
            std::string size_on_disk_path    = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/block_info/size_on_disk", w , queue_name.c_str() );
            std::string size_total_path      = au::str("worker[%d]/stream_manager/queues/queue[name=%s]/block_info/size", w , queue_name.c_str() );
            
            total_kvs += treeItem->getUInt64( kvs_path );
            total_size += treeItem->getUInt64( size_path );
            total_num_blocks += treeItem->getInt( num_blocks_path );
            total_rate_kvs += treeItem->getUInt64( rate_kvs_path );
            total_rate_size += treeItem->getUInt64( rate_size_path );
            
            total_size_on_memory    += treeItem->getUInt64( size_on_memory_path );
            total_size_on_disk      += treeItem->getUInt64( size_on_disk_path );
            total_size_total        += treeItem->getUInt64( size_total_path );
            
            _myModel->setItem(w, 1, new QStandardItem( QString( treeItem->get( key_path ).c_str()  ) ) );
            _myModel->setItem(w, 2, new QStandardItem( QString( treeItem->get( value_path ).c_str() ) ) );
            _myModel->setItem(w, 3, new QStandardItem( QString( treeItem->get( kvs_path ).c_str()  ) ) );
            _myModel->setItem(w, 4, new QStandardItem( QString( treeItem->get( size_path ).c_str() ) ) );
            _myModel->setItem(w, 5, new QStandardItem( QString( treeItem->get( num_blocks_path ).c_str()  ) ) );
            _myModel->setItem(w, 6, new QStandardItem( QString( treeItem->get( rate_kvs_path ).c_str() ) ) );
            _myModel->setItem(w, 7, new QStandardItem( QString( treeItem->get( rate_size_path ).c_str()  ) ) );

            _myModel->setItem(w, 8, new QStandardItem( QString( treeItem->get( size_total_path ).c_str()  ) ) );
            _myModel->setItem(w, 9, new QStandardItem( QString( treeItem->get( size_on_memory_path ).c_str()  ) ) );
            _myModel->setItem(w, 10, new QStandardItem( QString( treeItem->get( size_on_disk_path ).c_str()  ) ) );
            
        }
        
        setWorkersModel( _myModel );

        
        // Samsples
        plot_size->push( total_size );
        plot_kvs->push( total_kvs );
        plot_num_blocks->push( total_num_blocks );
        plot_rate_size->push( total_rate_size );
        plot_rate_kvs->push( total_rate_kvs );
        
        /*
        samples_size.push( total_size );
        // Update evolution plot
        m_model_evolution.clear();
        for (size_t row = 0; row < 100 ; ++row) 
        {
            m_model_evolution.setItem(row, 0, new QStandardItem( QString( au::str( "%lu" , samples_size.get(row) ).c_str() ) ));
        }
         */
        
        LM_M(("Pushing %lu" , total_size ));
        
        // Set main labels
        label_kvs->setText( au::str( total_kvs ).c_str() );
        label_size->setText( au::str( total_size ).c_str() );
        label_num_blocks->setText( au::str( total_num_blocks ).c_str() );
        label_rate_kvs->setText( au::str( total_rate_kvs ).c_str() );
        label_rate_size->setText( au::str( total_rate_size ).c_str() );
        
        
        // Update evolution disk_memory
        m_model_disk_memory.clear();
        m_model_disk_memory.setItem(0, 0, new QStandardItem( QString( au::str( "%lu", total_size_total - total_size_on_disk  ).c_str() ) ) );
        m_model_disk_memory.setItem(0, 1, new QStandardItem( QString( au::str( "%lu", total_size_on_memory + total_size_on_memory - total_size_total ).c_str() ) ) );
        m_model_disk_memory.setItem(0, 2, new QStandardItem( QString( au::str( "%lu", total_size_total - total_size_on_memory ).c_str() ) ) );
        
        // Delete copy of the item
        delete treeItem;
    }
        
    /*
     // Query string
     std::string query = query_command->text().toStdString();
     std::string fields = fields_command->text().toStdString();
     
     LM_M(("Query:  '%s'", query.c_str() ));    
     LM_M(("Fields: '%s'", fields.c_str() ));    
     
     // Get data set form delilah
    au::DataSet* dataSet = delilahConnection->delilah->getDataSet( query );
    
    if ( fields.length() > 0)
    {
        au::DataSetFilter *filter = new au::DataSetFilter( fields );
        au::DataSet* _dataSet = filter->transform( dataSet );
        
        delete dataSet;
        dataSet = _dataSet;
        delete filter;
    }
    
    // Set the model from data set
    setModel( createModel( dataSet ) ); 

    delete dataSet;
*/
}

void QueueWidget::change_queue()
{

   // Take the name of the queue
   queue_name =  entry_queue_name->text().toStdString();
   update();
}

void QueueWidget::setWorkersModel( QStandardItemModel* _workers_model )
{
    // Asign the model for the table
    table_workers->setModel( _workers_model );

    // Resize colum width with the content
    table_workers->resizeColumnsToContents();
    
    // Remove the previous one...
    if( workers_model )
        delete workers_model;
    
    workers_model = _workers_model;
    
}
