/* ****************************************************************************
*
* FILE                     LineViewPlugin.cpp
*
* DESCRIPTION			   Plugin to visualize data coming from the queues as a line graph
*
*/

#include "LineViewPlugin.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

LineViewPlugin::LineViewPlugin(QWidget* parent): DataViewPlugin(parent)
{
    /*ratePlotBox = new QGroupBox("Rate gr", this);
    ratePlotBox->setMinimumSize(200,200);*/



/*    //Feed connection
    connectButton = new QPushButton("Connect to this queue", this);
    connectButton->setCheckable(true);
    queueHeader = new QTextEdit(this);
    queueHeader->setReadOnly(true);
    queueFeed = new QTextEdit(this);
    queueFeed->setReadOnly(true);
    connectGroup = new QButtonGroup(this);
    connectGroup->setExclusive(false);
    connectNew = new QCheckBox("Only new data", this);
    connectClear = new QCheckBox("Remove popped data", this);
    clearFeedButton = new QPushButton("Clear", this);
    connectGroup->addButton(connectNew);
    connectGroup->addButton(connectClear);
 */   
    //graph
    plot = new LinePlot(this, "", "Rate (B/s)");
    rateCollection.resize(50);
    //Plot controls
    plotControlsLayout = new QHBoxLayout();
  plotControlsLayout->setObjectName("plotControlsLayout");  
    plotReset = new QPushButton("Reset", this);
    plotStop = new QPushButton("Pause", this);
    plotStop->setCheckable(true);
    plotNSamplesLabel = new QLabel("Num. of Samples:",this);
    plotNSamples = new QLineEdit("50", this);
    plotControlsLayout->addWidget(plotReset);
    plotControlsLayout->addWidget(plotStop);
    plotControlsLayout->addWidget(plotNSamplesLabel);
    plotControlsLayout->addWidget(plotNSamples);
    plotControlsLayout->addStretch();
    plot->layout->addLayout(plotControlsLayout);
    
/*    connect(connectButton, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
    connect(connectNew, SIGNAL(toggled(bool)), this, SLOT(onConnectNewClicked(bool)));
    connect(connectClear, SIGNAL(toggled(bool)), this, SLOT(onConnectClearClicked(bool)));
    connect(clearFeedButton, SIGNAL(clicked()), this, SLOT(clearFeed()));
    */
    connect(plotReset, SIGNAL(clicked()), this, SLOT(onPlotReset()));
    connect(plotNSamples, SIGNAL(editingFinished()), this, SLOT(onPlotNSamplesChanged()));
    
}

/*LineViewPlugin::~LineViewPlugin()
{
    //disconnect();
}*/

void LineViewPlugin::setData(std::string data)
{
        rateCollection.push(atof(data.c_str()));
        //rateCollection.takeSample();
        
        if(!plotStop->isChecked()) redrawPlot();
        
}

/*void ExtQueueViewer::updateHeader(std::string line)
{
    queueHeader->append(QString::fromUtf8(line.c_str()) + QString("\n"));
    //scroll contents to the bottom so added data is visible
    QScrollBar *sb = queueHeader->verticalScrollBar();
    sb->setValue(sb->maximum());

    queueHeader->update();
}
*/

void LineViewPlugin::redrawPlot()
{
   // Update the model for the plot....
    std::vector<std::string> labels;
    plot->clear();
    //int row = 0;
    //int desiredSamples = 50;
    //int increment = rateCollection.getNumSamples()/DesiredSamples;
    
    for ( unsigned int i = 0 ; i < rateCollection.elements.size() ; i++)
    {
        plot->set( i , 0 , rateCollection.get(i));
        //plot->set( i , row , rand()%10 );
        //row++;
    }
        
    //labels.push_back( it_value_collections->first );
        
    
    //plot->set_legend( "Levels..", labels );

}

void LineViewPlugin::onPlotReset()
{
   rateCollection.reset();
   redrawPlot();
}

void LineViewPlugin::onPlotNSamplesChanged()
{
    int value = plotNSamples->text().toInt();
    rateCollection.resize(value);
    redrawPlot();
}
} //namespace
