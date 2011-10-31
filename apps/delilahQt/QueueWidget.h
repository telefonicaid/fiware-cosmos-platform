#include <QtGui>
#include <sstream>

#include "au/RRT.h"
#include "au/string.h"

#include <KDChartChart>
#include <KDChartBarDiagram>
#include <KDChartLineDiagram>
#include <KDChartPieDiagram>

#include "Plot.h"

#include "ui_QueueWidget.h"


class QueueWidget : public QWidget , public Ui_QueueWidget
{
    
    std::string queue_name;  // Name of the queue we are monitoring
    
    QStandardItemModel *workers_model;      // Data Model for the worker table
    
    
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
    
    private slots:
    
    void update();
    
	void change_queue();
    
    
private:
    
    void setWorkersModel( QStandardItemModel* _myModel );
    
};

