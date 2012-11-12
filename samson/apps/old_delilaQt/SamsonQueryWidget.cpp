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



#include "samson/delilah/Delilah.h"
#include "DelilahConnection.h"     // DelilahConnection

#include "SamsonQueryWidget.h"  // Own interface

extern DelilahConnection* delilahConnection;

SamsonQueryWidget::SamsonQueryWidget( QWidget *parent ) : QWidget(parent)
{
  //Setup the 
  setupUi(this);

  // No initial model
  myModel = NULL;

  // Connections

  connect( updateButton , SIGNAL(clicked()) , this , SLOT(update()) );

}


void SamsonQueryWidget::update()
{
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
    
}

QStandardItemModel* SamsonQueryWidget::createModel( au::DataSet* dataSet )
{
    std::vector<std::string> fields;

    // Get all fields
    dataSet->getAllFields( fields );
    
    // Get number of records
    size_t num_records = dataSet->getNumRecords();

    LM_M(("Num columns: %lu", fields.size() ));    
    LM_M(("Num rows: %lu", num_records ));
    
    QStandardItemModel* _myModel = new QStandardItemModel();
    
    // Fill the model
    
    for (size_t column = 0; column < fields.size(); ++column) 
    {
        _myModel->setHorizontalHeaderItem( column , new QStandardItem( QString(  fields[column].c_str() ) ) );
        
        for (size_t row = 0; row < num_records ; ++row) {
            {
                std::string value = dataSet->getValue( row , fields[column] );   
                
                QStandardItem *item = new QStandardItem( QString( value.c_str() ) );
                _myModel->setItem(row, column, item);
            }
        }
    }
    
    return _myModel;
    
}

void SamsonQueryWidget::setModel( QStandardItemModel* _myModel )
{
    // Model of the tree
    tableView->setModel( _myModel );
    treeView->setModel( _myModel );
    
    tableView->resizeColumnsToContents();
    for ( int c = 0 ; c < 10 ; c++ )
        treeView->resizeColumnToContents(c);
    
    // Remove the previous one...
    if( myModel )
        delete myModel;
    
    myModel = _myModel;
    
}

QStandardItemModel* SamsonQueryWidget::getExampleModel()
{
    
    // Model
    QStandardItemModel* _myModel = new QStandardItemModel();
    
    // Header
    _myModel->setHorizontalHeaderItem( 0 , new QStandardItem( QString("Name") ) );
    _myModel->setHorizontalHeaderItem( 1 , new QStandardItem( QString("Surname") ) );
    
    /*    
     // Fill the model
     for (int row = 0; row < 10; ++row) {
     for (int column = 0; column < 2; ++column) 
     {
     QStandardItem *item = new QStandardItem( QString("Hola") );
     myModel.setItem(row, column, item);
     }
     }
     */
    
    
    
    for (int i = 0; i < 4; ++i) 
    {
        QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
        for (int j = 0 ; j < 5 ; j++ )
        {
            QStandardItem *_item = new QStandardItem(QString(" sub item %0").arg(i));
            item->appendRow( _item );
        }
        
        _myModel->appendRow(item);
    } 
    
    return _myModel;
    
}

