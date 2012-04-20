

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

extern samson::SamsonClient samson_client;

extern char title[1024];
extern char composed_title[1024];
extern time_t last_timestamp;
extern char x_title[1024];
extern char y_title[1024];

extern char concept[1024];
extern int refresh_time;
extern double ntimes_real_time;

extern au::map<std::string , au::ContinuousValueCollection<double> > value_collections;
extern au::map<std::string , au::ContinuousValueCollection<time_t> > time_collections;
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

  std::vector<std::string> labels;
  plot->clear();

  int row = 0;


  for ( it_value_collections = value_collections.begin() ; it_value_collections != value_collections.end() ; ++it_value_collections )
  {
    au::ContinuousValueCollection<double>* vc = it_value_collections->second;

    LM_M(("Plotting for collection:%s with %d values", it_value_collections->first.c_str(), vc->getNumSamples()));
    for ( int i = 0 ; i < vc->getNumSamples() ; i++)
    {
      //if (i%10 == 0)
      {
        plot->set( i, row , vc->getSample(i) );
        //plot->set( i , row , rand()%10 );
      }
    }
    LM_M(("Set %d values for plot", vc->getNumSamples()));

    bool label_found = false;
    for (unsigned int i = 0; (i < labels.size()); i++)
    {
      if (labels[i] == it_value_collections->first)
      {
        label_found = true;
        break;
      }
    }
    if (label_found == false)
    {
      labels.push_back( it_value_collections->first );
    }
    row++;
  }

  //if ((first_legend == true) && (labels.size() > 0))
  {
    plot->set_legend( "Levels..", labels );
    first_legend = false;
  }

}

