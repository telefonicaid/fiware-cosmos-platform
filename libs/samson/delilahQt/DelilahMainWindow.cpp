/* ****************************************************************************
 *
 * FILE                     DelilahMainWindow.cpp
 *
 * DESCRIPTION			   Main widget for delilahQt
 *
 */

#include "DelilahMainWindow.h" // Own interface

#include <QTimer>
#include <QtGui>

#include <sstream>

namespace samson
{	
    DelilahMainWindow::DelilahMainWindow()
    {
        layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

        queuesText = new QLabel(this);
        enginesText = new QLabel(this);
        
        layout->addWidget(queuesText);
        layout->addWidget(enginesText);
        
        queuesText->show();
        enginesText->show();
        
        exitAction = new QAction(tr("E&xit"), this);
        aboutAction = new QAction(tr("A&bout"), this);
        fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(aboutAction);
        fileMenu->addAction(exitAction);
        
        QObject::connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
        connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    }
    
    void DelilahMainWindow::about()
    {
        QMessageBox::about( this, tr("Samson"), tr("Samson version 0.7") );
    }
    
}
