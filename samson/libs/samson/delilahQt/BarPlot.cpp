


#include "BarPlot.h" // Own interface

#include <QApplication>
#include <KDChartWidget>
#include <KDChartLineDiagram>
#include <KDChartPosition>
#include <KDChartLegend>

BarPlot::BarPlot( QFrame *_widget , const char* x_title ,  const char* y_title )
{
   // Keep a pointer to the group ( change the title in the future )
	widget = _widget;
	
    diagram = new KDChart::BarDiagram;
    diagram->setModel(&m_model);
    
    diagram->setPen( 0 , QPen( Qt::red, 0 ) );
    diagram->setPen( 1 , QPen( Qt::black, 0 ) );
    diagram->setPen( 2 , QPen( Qt::green, 0 ) );
    
/*    KDChart::BackgroundAttributes ba;
    ba.setBrush(  Qt::white );
    ba.setVisible( true );*/
   // diagram->setPaper(  QBrush(Qt::white) );    	
    
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
    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );
    
    m_chart.coordinatePlane()->replaceDiagram(diagram);
    

	// Example to show something
	/*for (int i = 0; i  < 10 ; i++)
		for( int j = 0 ; j < 2 ; j++ )
			set( i , j , rand()%30 );*/


	//set_legend( "Main title" , "line 1" , "line 2" );

    
	// Add the plot in the widget
    layout = new QVBoxLayout();
    widget->setLayout(layout);
    layout->addWidget(&m_chart);
    m_chart.setGlobalLeadingTop( 10 );
	//KDChart::setLayout(l);



}

BarPlot::~BarPlot()
{
   delete diagram;
}

void BarPlot::set_legend( std::string title ,  std::string label_1 )
{
   std::vector<std::string> labels;
   labels.push_back( label_1 );

   set_legend( title , labels );
}

void BarPlot::set_legend( std::string title ,  std::string label_1 , std::string label_2 )
{
   std::vector<std::string> labels;
   labels.push_back( label_1 );
   labels.push_back( label_2 );

   set_legend( title , labels );
}

void BarPlot::set_legend( std::string title ,  std::vector<std::string>& labels )
{
    /*
   KDChart::Legend* legend = new KDChart::Legend( diagram, &m_chart );

   legend->setShowLines( true );
   //legend->setSpacing( 5 );

   legend->setTitleText( title.c_str() );

   for ( size_t i=0; i<labels.size() ; i++ )
	  legend->setText( i , labels[i].c_str() );


   legend->setPosition( KDChart::Position::South );
   legend->setAlignment( Qt::AlignCenter );
   legend->setOrientation( Qt::Horizontal );
*/
}

void BarPlot::set( int r , int c , double v )
{
    //LM_M(("Plot %d %d %f" , r , c , v ));
    m_model.setItem(r, c, new QStandardItem( QString( au::str( "%f", v).c_str() ) ) );
}

void BarPlot::clear()
{
    m_model.clear();
}


