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
#include <sstream>


#include "tables/Table.h"
#include "tables/Tree.h"

#include "ui_SamsonNodeWidget.h"


namespace au
{
    class TreeItem;
}

class SamsonNodeWidget : public QWidget , public Ui_SamsonNodeWidget
{
    
    QStandardItemModel *myModel;

    std::string title;
    size_t uptime;
    
    Q_OBJECT
    
public:
    
    SamsonNodeWidget(QWidget *parent = 0);

    void setTitle( std::string _title );
    void updateTitle( );
    void update( au::tables::TreeItem *treeItem );    
    
    private slots:
    
    
    
private:
    

    
};

