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

Plot::Plot( QGroupBox *_box )
{
   box = _box;

    KDChart::LineDiagram* diagram = new KDChart::LineDiagram;
    diagram->setModel(&m_model);
    
    diagram->setPen( 0 , QPen( Qt::red, 0 ) );
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

    KDChart::TextAttributes ta = yAxis->textAttributes();
    KDChart::Measure me( ta.fontSize() );
    me.setValue( me.value() * 3.0 );
    ta.setFontSize( me );
    yAxis->setTextAttributes( ta );
    
    
    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );
    
    m_chart.coordinatePlane()->replaceDiagram(diagram);
    
    QVBoxLayout* l = new QVBoxLayout( box );
    l->addWidget(&m_chart);
    //box->setLayout(l);
    
    
    
    
    samples = 0;
}

void Plot::set( int r , int c , size_t v )
{
    m_model.setItem(r, c, new QStandardItem( QString( au::str( "%lu", v).c_str() ) ) );
}


void Plot::push( size_t value , bool complete_update )
{
    // Samples for size plot
    samples_last_minute.push( value );
    
    if( samples > 0 )
        if( (samples % 60) == 0 )
			samples_last_hour.push( samples_last_minute.get(0) );
    
    if( samples > 0 )
        if( ( samples % 3600 ) == 0 )
			samples_last_day.push( samples_last_hour.get(0) );
    
    samples++;

	if( !box->isVisible() )
	   return;
    
	if( !complete_update )
	   return;

    // Update model
    // Update evolution plot
    m_model.clear();
    
    for (size_t row = 0; row < (24+60+60) ; ++row) 
    {
        if( row < 24 )
			set(row, 0, samples_last_day.get(row) );
        else if( row < (24+60) )
        {
			int h = row - 24;
			set(row, 0, samples_last_hour.get(h) );
        }
        else
        {
			int s = row - 60 - 24;
            
			set(row, 0, samples_last_minute.get(s) );
        }
    }
}
