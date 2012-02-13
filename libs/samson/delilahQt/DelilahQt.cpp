/* ****************************************************************************
 *
 * FILE                     DelilahQt.cpp
 *
 * DESCRIPTION			   QT terminal for delilah
 *
 */

#include "DelilahQt.h" // Own interface
#include "DelilahMainWindow.h"
#include "TableViewer.h"

#include <QTimer>
#include <QtGui>

#include <sstream>

namespace samson
{	
    void DelilahQt::updateData()
    {
        {
            au::tables::Table* table = database.getTable("queues");
            QString str;
            if( !table )
            {
                str = QString("No table queues");
                //std::cout << "No table queues" << std::endl;
                emit queuesTextValueChanged(str);
            }
            else
            {
                std::stringstream tmp;
                for (unsigned int i = 0; i< table->getNumRows(); i++)
                {
                    tmp << "Queue: " << table->getValue(i,"name") << " - Size on Memory: " << table->getValue(i,"block_list/block_info/size_on_memory") << std::endl;
                }
                str = QString(tmp.str().c_str());
                //str = QString(table->str().c_str());
                //std::cout << "Table queues: " << table->str() << std::endl;
                emit queuesTextValueChanged(str);
            }
        }
            
        {
            au::tables::Table* table = database.getTable("engines");
            QString str;
            if( !table )
            {
                str = QString("No table engines");
                //std::cout << "No table queues" << std::endl;
                emit enginesTextValueChanged(str);
            }
            else
            {
                std::stringstream tmp;
                tmp << "Table engines with " << table->getNumRows() << " rows";
                str = QString(tmp.str().c_str());
                //std::cout << "Table engines with " << table->getNumRows() << " rows" << std::endl;
                //std::cout << "Table engines: " << table->str() << std::endl;
                emit enginesTextValueChanged(str);
            }
        }            
        
    }
    
    DelilahQt::DelilahQt( NetworkInterface *network ) : QObject(), Delilah( network )
    {
        
    }
    
    void DelilahQt::run()
    {
        
        int argn = 0;
        QApplication application(argn, NULL);
        QTimer* timer =  new QTimer();
        timer->start(1000);
            
        DelilahMainWindow* mainWindow = new DelilahMainWindow;
        mainWindow->show();

        connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
        connect(timer, SIGNAL(timeout()), mainWindow, SLOT(update()));
        //connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow->enginesText, SLOT(setText(const QString&)));
        //connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow->queuesText, SLOT(setText(const QString&)));
        application.exec();
        
    }
    
}
