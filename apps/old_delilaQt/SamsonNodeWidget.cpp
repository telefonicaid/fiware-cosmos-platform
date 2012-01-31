#include <QtGui>


// Include header files for application components.
// ...

#include <algorithm>

#include "au/time.h"
#include "au/string.h"

#include "tables/Tree.h"

#include "samson/delilah/Delilah.h"

#include "SamsonNodeWidget.h"  // Own interface

extern samson::Delilah* delilah;

SamsonNodeWidget::SamsonNodeWidget( QWidget *parent ) : QWidget(parent)
{
    title = "";
    uptime = 0;
    
  //Setup the 
  setupUi(this);

}

void SamsonNodeWidget::setTitle( std::string _title )
{
    title = _title;
    updateTitle();
}

void SamsonNodeWidget::updateTitle()
{
    std::string full_title = title + " " + au::time_string( uptime );
    groupBox->setTitle( tr( full_title.c_str() ) );
}

void SamsonNodeWidget::update( au::tables::TreeItem *treeItem )
{
    if( !treeItem )
        return;
    
    size_t used_memory  = treeItem->getUInt64( "engine_system/memory_manager/used_memory" );
    size_t memory       = treeItem->getUInt64( "engine_system/memory_manager/memory" ); 
    uptime              = treeItem->getUInt64("engine_system/uptime" );
    
    updateTitle();
    
    memory_progressBar->setRange( 0 , 100);
    memory_progressBar->setValue( 100.0 * (double) used_memory / (double) memory );
    memory_progressBar->setTextVisible(true);
    memory_description->setText( QString( au::str("%s / %s" , au::str( used_memory ).c_str() , au::str( memory ).c_str() ).c_str() ) );
    
    size_t num_running_processes    = treeItem->getUInt64( "engine_system/process_manager/num_running_processes" );
    size_t num_processes            = treeItem->getUInt64( "engine_system/process_manager/num_processes" ); 
    
    process_progressBar->setRange( 0 , num_processes );
    process_progressBar->setValue( num_running_processes );
    process_progressBar->setTextVisible(true);
    process_description->setText( QString( au::str("%s / %s" , au::str( num_running_processes ).c_str() , au::str( num_processes ).c_str() ).c_str() ) );

    
    size_t disk_read_rate    = treeItem->getUInt64( "engine_system/disk_manager/statistics/read/rate" );
    size_t disk_write_rate    = treeItem->getUInt64( "engine_system/disk_manager/statistics/write/rate" );
    size_t disk_rate = disk_read_rate + disk_write_rate;
    size_t max_rate = std::max( (size_t)300000000 , disk_rate ); 
    size_t disk_ops = treeItem->getUInt64( "engine_system/disk_manager/num_pending_operations" );
    
    disk_progressBar->setRange( 0 , max_rate );
    disk_progressBar->setValue( disk_rate );
    disk_progressBar->setTextVisible(true);
    disk_description->setText( QString( au::str("R: %s W: %s Ops: %s" , au::str( disk_read_rate ,"Bs" ).c_str() , au::str( disk_write_rate ,"Bs" ).c_str() , au::str( disk_ops ).c_str() ).c_str() ) );

    
    size_t network_read_rate    = treeItem->getUInt64( "network/statistics/read/rate" );
    size_t network_write_rate    = treeItem->getUInt64( "network/statistics/write/rate" );
    size_t network_rate = disk_read_rate + disk_write_rate;
    size_t network_max_rate = std::max( (size_t)100000000 , network_rate ); 
    
    network_progressBar->setRange( 0 , network_max_rate );
    network_progressBar->setValue( network_rate );
    network_progressBar->setTextVisible(true);
    network_description->setText( QString( au::str("R: %s W: %s" , au::str( network_read_rate ,"Bs" ).c_str() , au::str( network_write_rate ,"Bs" ).c_str() ).c_str() ) );
    
    
    
    
    
}




