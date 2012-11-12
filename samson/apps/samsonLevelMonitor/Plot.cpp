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



#include "Plot.h" // Own interface

#include <QApplication>
#include <KDChartWidget>
#include <KDChartBarDiagram>
#include <KDChartPosition>
#include <KDChartLegend>

extern int refresh_time;
extern bool graph_time;
extern double ntimes_real_time;
extern bool logX;
extern bool logY;

Plot::Plot( QGroupBox *_box , const char* x_title ,  const char* y_title )
{
   // Keep a pointer to the group ( change the title in the future )
	box = _box;
	
    diagram = new KDChart::LineDiagram;
    diagram->setModel(&m_model);
    
    //diagram->setPen( 0 , QPen( Qt::red, 0 ) );
    //diagram->setPen( 1 , QPen( Qt::black, 0 ) );
    //diagram->setPen( 2 , QPen( Qt::green, 0 ) );
    	
    // Margins...
    //m_chart.setGlobalLeading( 10,  10,  10,  10 );    
    
    // Axis
    // create and position axis
    KDChart::CartesianAxis *xAxis = new KDChart::CartesianAxis( diagram );
    KDChart::CartesianAxis *yAxis = new KDChart::CartesianAxis( diagram );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    //xAxis->setTitleText ( "X title" );
    //yAxis->setTitleText ( "Y title" );

    
    xAxis->setTitleText( QString( x_title ) );
    yAxis->setTitleText( QString( y_title ) );


    /*
    KDChart::TextAttributes ta = yAxis->textAttributes();
    KDChart::Measure me( ta.fontSize() );
    me.setValue( me.value() * 3.0 );
    ta.setFontSize( me );
    yAxis->setTextAttributes( ta );
*/

    if (graph_time)
    {
    QStringList times = xAxis->labels();
    QStringList times_short = xAxis->shortLabels();
    //LM_M(("xAxis with %d labels", times.size()));
    if (times.size() > 0)
    {
    for (int i = 0; (i < times.size()); i++)
    {
      int secs = times.at(i).toInt();
      secs = (100 - secs)*(refresh_time*ntimes_real_time);
      int hours = secs/3600;
      secs -= (hours * 3600);
      int minutes = secs/60;
      secs -= (minutes * 60);

      char new_label[33];
      sprintf(new_label, "-%d:%02d:%02d", hours, minutes, secs);
      //LM_M(("new label at pos:%d -> '%s'", i, new_label));
      times.replace(i,QString::fromUtf8(new_label));
    }
    }
    else
    {
      for (int i = 0; (i < 100); i++)
      {
        int secs = (100 - i)*(refresh_time*ntimes_real_time);
        int hours = secs/3600;
        secs -= (hours * 3600);
        int minutes = secs/60;
        secs -= (minutes * 60);

        char new_label[33];
        sprintf(new_label, "-%d:%02d:%02d", hours, minutes, secs);
        //LM_M(("new label at pos:%d -> '%s'", i, new_label));

        times << new_label;

        sprintf(new_label, "-%dm", secs/60);
        //LM_M(("new label at pos:%d -> '%s'", i, new_label));
        times_short << new_label;
      }
    }
    //times << 90*refresh_time*ntimes_real_time << " " <<  70*refresh_time*ntimes_real_time << " " <<  50*refresh_time*ntimes_real_time <<  " " << 30*refresh_time*ntimes_real_time <<  " " << 10*refresh_time*ntimes_real_time ;
    //times << "9000" << "7000" << "5000" << "3000" << "1000";
    xAxis->setLabels( times );
    xAxis->setShortLabels( times );
    }

    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );
    


    m_chart.coordinatePlane()->replaceDiagram(diagram);

    // Logarithmic scales
    if (logX == true)
    {
        LM_M(("Setting logarithmic scale for X axis"));
        KDChart::CartesianCoordinatePlane *plane_tmp = static_cast <KDChart::CartesianCoordinatePlane*>
(m_chart.coordinatePlane());
        plane_tmp->setAxesCalcModeX(KDChart::AbstractCoordinatePlane::Logarithmic);
    }
    if (logY == true)
    {
        LM_M(("Setting logarithmic scale for Y axis"));
        KDChart::CartesianCoordinatePlane *plane_tmp = static_cast <KDChart::CartesianCoordinatePlane*>
 (m_chart.coordinatePlane());
         plane_tmp->setAxesCalcModeY(KDChart::AbstractCoordinatePlane::Logarithmic);
    }

    
/*
	// Example to show something
	for (int i = 0; i  < 10 ; i++)
		for( int j = 0 ; j < 2 ; j++ )
			set( i , j , rand()%30 );


	set_legend( "Main title" , "line 1" , "line 2" );
*/
    
	// Add the plot in the box
    QVBoxLayout* l = new QVBoxLayout( box );
    l->addWidget(&m_chart);
    m_chart.setGlobalLeadingTop( 10 );
	//KDChart::setLayout(l);

    // Add a legend and set it up
/*    KDChart::Legend* legend = new KDChart::Legend( diagram, &m_chart );
    legend->setPosition( KDChart::Position::NorthWest );
    legend->setAlignment( Qt::AlignCenter );
    legend->setShowLines( false );
    //legend->setTitleText(tr( "Bars" ) );
    legend->setOrientation( Qt::Vertical );
    m_chart.addLegend( legend );*/

    legend_ = new KDChart::Legend( diagram, &m_chart );
    m_chart.addLegend( legend_ );


}

Plot::~Plot()
{
   delete diagram;
}

void Plot::set_legend( std::string title ,  std::string label_1 )
{
   std::vector<std::string> labels;
   labels.push_back( label_1 );

   set_legend( title , labels );
}

void Plot::set_legend( std::string title ,  std::string label_1 , std::string label_2 )
{
   std::vector<std::string> labels;
   labels.push_back( label_1 );
   labels.push_back( label_2 );

   set_legend( title , labels );
}

void Plot::set_legend( std::string title ,  std::vector<std::string>& labels )
{
   legend_->setShowLines( true );
   //legend->setSpacing( 5 );

   legend_->setTitleText( title.c_str() );

   //LM_M(("set_legend with %d lables", labels.size()));
   for ( size_t i=0; i<labels.size() ; i++ )
   {
     //LM_M(("Add legend at pos %d with label:'%s'", i, labels[i].c_str() ));
	  legend_->setText( i , labels[i].c_str() );
   }

   legend_->setPosition( KDChart::Position::West );
   legend_->setAlignment( Qt::AlignCenter );
   legend_->setOrientation( Qt::Vertical );
}

void Plot::set( int r , int c , double v )
{
    //LM_M(("Plot %d %d %f" , r , c , v ));
    m_model.setItem(r, c, new QStandardItem( QString( au::str( "%f", v).c_str() ) ) );
}

void Plot::clear()
{
    m_model.clear();
}


