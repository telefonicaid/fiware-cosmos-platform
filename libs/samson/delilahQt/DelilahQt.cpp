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
    void DelilahQt::setData()
    {
        {
            std::vector<std::string> names;
            std::vector<std::string> values;
            au::tables::Table* table = database.getTable("engines");
            QString str;
            if( !table || table->getNumRows() < 2 )
            {
                names.push_back(std::string("No queues data"));
                values.push_back(std::string("--"));
                
                //std::cout << "No table queues" << std::endl;
            }
            else
            {
                std::stringstream tmp;
                for(unsigned int i = 0; i< table->getNumColumns(); i++)
                {
                    names.push_back(table->getValue(0, i));
                    for (unsigned int j = 1; j< table->getNumRows(); j++)
                    {
                        values.push_back( table->getValue(j, i));
                    }
                    //str = QString(tmp.str().c_str());
                    //emit queuesTextValueChanged(str);
                }
            }
            mainWindow->tableViewer->setData(names, values);
        }
            
    }

    void DelilahQt::updateData()
    {
        {
            std::vector<std::string> values;
            au::tables::Table* table = database.getTable("engines");
            QString str;
            if( table && table->getNumColumns() >= 2)
            {
                std::stringstream tmp;
                for(unsigned int i = 1; i< table->getNumRows(); i++)
                {
                   for (unsigned int j = 0; j < table->getNumColumns(); j++)
                   {
                       values.push_back( table->getValue(i, j));
                    }
                   std::cout << "Copiados valores" << std::endl;
                 //std::cout << "Table queues: " << table->str() << std::endl;
                   //emit queuesTextValueChanged(str);
               }
               mainWindow->tableViewer->updateData(values);
            }

        /*{
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
            */
        }   
                 
        
    }
    
    DelilahQt::DelilahQt( NetworkInterface *network ) : Delilah( network )
    {
        
    }
    
    void DelilahQt::run()
    {
        
        int argn = 0;
        QApplication application(argn, NULL);
        QTimer* timer =  new QTimer();
        timer->start(1000);
            
        mainWindow = new DelilahMainWindow;
        
        setData();
        //mainWindow->tableViewer->setData(names, values);
        mainWindow->show();

        connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
        connect(timer, SIGNAL(timeout()), mainWindow, SLOT(update()));
        //connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow->enginesText, SLOT(setText(const QString&)));
        //connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow->queuesText, SLOT(setText(const QString&)));
        application.exec();
        
    }
    
}
