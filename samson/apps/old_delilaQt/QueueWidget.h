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

#include "au/RRT.h"
#include "au/string.h"

#include <KDChartChart.h>
#include <KDChartBarDiagram.h>
#include <KDChartLineDiagram.h>
#include <KDChartPieDiagram.h>

#include "Plot.h"
#include "UpdateElements.h"  // delilaQTElement

#include "WorkerDataController.h"

#include "ui_QueueWidget.h"


class QueueWidget : public QWidget , public Ui_QueueWidget , DelilahQTElement
{
    
    std::string queue_name;                         // Name of the queue we are monitoring
    WorkerDataController worker_data_controller; // Controller to keep updated values from all workers
    
    // Plot memory-disk
    QStandardItemModel m_model_disk_memory;
    KDChart::Chart m_chart_disk_memory;
    
    Plot *plot_size;
    Plot *plot_kvs;
    Plot *plot_num_blocks;
    Plot *plot_rate_size;
    Plot *plot_rate_kvs;
    
    Q_OBJECT
    
public:
    
    QueueWidget(QWidget *parent = 0);

	// Update with the top level element in XML monitorization
	void update( au::tables::TreeItem *item , bool complete_update );

    
    private slots:
    
	void change_queue();
    
    
private:
    
    void setup();
    
};

