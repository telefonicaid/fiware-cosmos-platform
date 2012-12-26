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


#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // memcpy
#include <iostream>     // std::cout

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"

#include "au/containers/map.h"
#include "au/RRT.h"             // au::ValueCollection 
#include "au/mutex/Token.h"
#include "au/string.h"              // au::str()
#include "au/mutex/TokenTaker.h"

#include "samson/client/SamsonClient.h"         // samson::SamsonClient

#include "common.h"     // Common defintion

#include "MainWindow.h" // Own interface

extern char title[1024];
extern char composed_title[1024];
extern char x_title[1024];
extern char y_title[1024];

extern char concept[1024];
extern int refresh_time;

extern au::map<std::string , au::ContinuousValueCollection<double> > value_collections;
extern au::Token token_value_collections;

extern char title[1024];
extern char concept[1024];

MainWindow::MainWindow( QWidget *parent ) : QMainWindow(parent)
{

    //Setup the
    setupUi(this);

    // Connections
    // connect(samsonQueryPushButton, SIGNAL(clicked()), this, SLOT(open_samson_query()));
    //connect( treeUpdateButton , SIGNAL(clicked()) , this , SLOT( updateTree() ) );

    // Set the top label with information about the connected queue
    if( strcmp( title , "no_title") == 0)
        label_info->setText( au::str("Monitoring %s" , concept ).c_str() );
    else
        label_info->setText( title );

    // General update model
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    timer->start(refresh_time * 1000);


    // Create the plot
    plot = new Plot( groupBox_plot , x_title , y_title );
    first_legend = true;


}

void MainWindow::updateTimer()
{
    au::TokenTaker tt(&token_value_collections);

    // Take a sample for all of them
    au::map<std::string , au::ContinuousValueCollection<double> >::iterator it_value_collections;
    for ( it_value_collections = value_collections.begin() ; it_value_collections != value_collections.end() ; ++it_value_collections )
        it_value_collections->second->takeSample();

    LM_M(("New title with timestamp:%s", composed_title));
    label_info->setText( composed_title );

    // Update the model for the plot....


    plot->clear();

    int row = 0;


    for ( it_value_collections = value_collections.begin() ; it_value_collections != value_collections.end() ; ++it_value_collections )
    {
             au::ContinuousValueCollection<double>* vc = it_value_collections->second;

            //LM_M(("Plotting for collection:%s with %d values", it_value_collections->first.c_str(), vc->getNumSamples()));
            for ( int i = 0 ; i < vc->getNumSamples() ; i++)
            {
                plot->set( i, row , vc->getSample(i) );
            }
            //LM_M(("Set %d values for plot", vc->getNumSamples()));

            bool label_found = false;
            for (unsigned int i = 0; (i < labels_.size()); i++)
            {
                if (labels_[i] == it_value_collections->first)
                {
                    //LM_M(("Detects label %s at pos %u", labels_[i].c_str(), i));
                    label_found = true;
                    break;
                }
            }
            if (label_found == false)
            {
                //LM_M(("Adds label %s after checking %u entries", it_value_collections->first.c_str(), labels_.size()));
                labels_.push_back( it_value_collections->first );
            }
            row++;
    }

    //if ((first_legend == true) && (labels.size() > 0))
    {
        plot->set_legend( "Levels..", labels_ );
        first_legend = false;
    }

}

