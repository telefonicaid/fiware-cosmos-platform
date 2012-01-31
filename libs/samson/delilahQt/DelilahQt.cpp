/* ****************************************************************************
 *
 * FILE                     DelilahQt.h
 *
 * DESCRIPTION			   QT terminal for delilah
 *
 */

#include "DelilahQt.h" // Own interface


namespace samson
{	
    DelilahQt::DelilahQt( NetworkInterface *network ) : Delilah( network )
    {
        
    }
    
    void DelilahQt::run()
    {
        
        while( true )
        {
            {
                au::tables::Table* table = database.getTable("queues");
                if( !table )
                    std::cout << "No table queues" << std::endl;
                else
                    std::cout << "Table queues with " << table->getNumRows() << " rows" << std::endl;
            }
            
            {
                au::tables::Table* table = database.getTable("engines");
                if( !table )
                    std::cout << "No table engines" << std::endl;
                else
                    std::cout << "Table engines with " << table->getNumRows() << " rows" << std::endl;
            }            
            
            sleep(1);
            
        }
        
    }
    
}
