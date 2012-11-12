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
#include <iostream>

// Include header files for application components.
// ...

#include "ui_SamsonConnect.h"
#include <assert.h>

class SamsonConnect : public QWidget , public Ui_SamsonConnect
{
    Q_OBJECT
    
public:
    
    SamsonConnect(QWidget *parent = 0);
    
    // Set a particular error during connection...
    void setError( std::string error_message);
    
    void SetConnectionMessage( std::string message );
    
    private slots:
    
    void samson_connect();
    void samson_connect_localhost();
    
    
};

