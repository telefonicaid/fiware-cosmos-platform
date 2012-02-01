/* ****************************************************************************
 *
 * FILE                     DelilahQt.h
 *
 * DESCRIPTION			   QT terminal for delilah
 *
 */

#include "DelilahQt.h" // Own interface

#include <QTimer>
#include <QtGui>

#include <sstream>

namespace samson
{	
    DelilahMainWindow::DelilahMainWindow(QWidget *parent ) : QWidget(parent)
    {
        layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

        queuesText = new QLabel(this);
        enginesText = new QLabel(this);
        
        layout->addWidget(queuesText);
        layout->addWidget(enginesText);
        
        queuesText->show();
        enginesText->show();
    }
    
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
                tmp << "Table queues with " << table->getNumRows() << " rows";
                str = QString(tmp.str().c_str());
                //std::cout << "Table queues with " << table->getNumRows() << " rows" << std::endl;
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
                emit enginesTextValueChanged(str);
            }
        }            
        
    }
    
    DelilahQt::DelilahQt( NetworkInterface *network ) : Delilah( network )//, QObject()
    {
        
    }
    
    void DelilahQt::run()
    {
        
        int argn = 0;
        QApplication application(argn, NULL);
        QTimer* timer =  new QTimer();
        timer->setInterval(1000);
            
        DelilahMainWindow mainWindow;

        connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
        connect(this, SIGNAL(enginesTextValueChanged(const QString&)), mainWindow.enginesText, SLOT(setText(const QString&)));
        connect(this, SIGNAL(queuesTextValueChanged(const QString&)), mainWindow.queuesText, SLOT(setText(const QString&)));
        application.exec();
        
    }
    
}
