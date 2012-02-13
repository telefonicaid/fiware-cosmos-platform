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
        tabs = new QTabWidget(this);
        setCentralWidget(tabs);
        
        tab1 = new QWidget();

        mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, tab1);
        /*queuesLayout = new QBoxLayout(QBoxLayout::TopToBottom);
        enginesLayout = new QBoxLayout(QBoxLayout::TopToBottom);
        queuesBox = new QGroupBox("Queues", tab1);
        enginesBox = new QGroupBox("Engine",tab1);
        mainLayout->addWidget(queuesBox);
        mainLayout->addWidget(enginesBox);*/

        /*queuesText = new QLabel();
        queuesLayout->addWidget(queuesText);
        queuesLayout->addStretch();
        enginesText = new QLabel();
        enginesLayout->addWidget(enginesText);
        enginesLayout->addStretch();
        queuesBox->setLayout(queuesLayout);
        enginesBox->setLayout(enginesLayout);
        
        queuesText->show();
        enginesText->show();
        */
        tableViewer = new TableViewer(tab1, "Engine");
        mainLayout->addWidget(tableViewer);
                
        tabs->addTab(tab1, tr("Engine"));
        
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
