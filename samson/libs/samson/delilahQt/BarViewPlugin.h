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
#ifndef _H_BarViewPlugin
#define _H_BarViewPlugin

/* ****************************************************************************
*
* FILE                     BarViewPlugin.h
*
* DESCRIPTION			   Plugin to visualize data coming from the queues in the form of a Bar graph
*
*/

#include "DataViewPlugin.h"
#include "Plot.h"

#include "au/RRT.h"             // au::ValueCollection 

#include <QtGui>
#include <vector>
#include <string>

namespace samson {


class BarViewPlugin: public DataViewPlugin
{
        Q_OBJECT
    public:
        
        BarViewPlugin(QWidget* parent = 0);
        //~BarViewPlugin();

    private:
        
        BarPlot* plot;
        //Data for the plot
        au::ResizableRRT<unsigned long> rateCollection;
        //Plot controls
        QHBoxLayout* plotControlsLayout;
        QPushButton* plotReset;
        QPushButton* plotStop;
        QLabel* plotNSamplesLabel;
        QLineEdit* plotNSamples;
        
        //QGroupBox* ratePlotBox;
        

    public slots:
        void setData(std::string data);
        void redrawPlot();
        void onPlotReset();
        void onPlotNSamplesChanged();
        
};

} //namespace

#endif //_H_BarViewPlugin
