#include <QtGui>
#include <sstream>

// Include header files for application components.
// ...



#include "au/Table.h"

#include "ui_MainWindow.h"              // Parent class

#include "UpdateElements.h"

namespace au {
    class TreeItem;
}

class SamsonNodeWidget;

class MainWindow : public QMainWindow , public Ui_MainWindow , public DelilahQTElement
{
    
    bool flag_init;   // Flag to indicate if the window has been inittiazted

    // Vector of elements to display information about workers
    std::vector<SamsonNodeWidget *> workerSamsonNodeWidgets;
    int num_workers;  // Number  of workers
    
    Q_OBJECT
    
public:
    
    MainWindow(QWidget *parent = 0);
    
private slots:
    
    void open_samson_query();
    void updateTimer();
    void updateTree();    
    void show_queues();
    void show_stream_operations();
    
private:
    
    static QStandardItemModel* itemModelFromTreeItem( au::tables::TreeItem* treeItem );
    static QStandardItem* itemFromTreeItem( au::tables::TreeItem* treeItem );
    
public:
    
    void init();
    void update(); // Update all information from delilah

    
};

