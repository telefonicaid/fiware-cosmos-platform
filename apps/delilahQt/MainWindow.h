#include <QtGui>
#include <sstream>

// Include header files for application components.
// ...



#include "au/Table.h"

#include "ui_MainWindow.h"              // Parent class


namespace au {
    class TreeItem;
}

class SamsonNodeWidget;

class MainWindow : public QMainWindow , public Ui_MainWindow
{
    
    bool flag_init;  // Flag to indicate if the window has been inittiazted
    int num_workers;

    // Reference to the SamsonNodeWidget to plot status
    SamsonNodeWidget *controllerSamsonNodeWidget;
    std::vector<SamsonNodeWidget *> workerSamsonNodeWidgets;
    SamsonNodeWidget *delilahSamsonNodeWidget;
    
    Q_OBJECT
    
public:
    
    MainWindow(QWidget *parent = 0);
    
private:
    
    QStandardItemModel *myModel;
    void setModel( QStandardItemModel *myModel );
    
    
private slots:
    
    void open_samson_query();
    void updateTimer();
    void updateTree();    
    
private:
    
    static QStandardItemModel* itemModelFromTreeItem( au::TreeItem* treeItem );
    static QStandardItem* itemFromTreeItem( au::TreeItem* treeItem );
    
public:
    
    void init();
    void update( au::TreeItem *treeItem );
    
};

