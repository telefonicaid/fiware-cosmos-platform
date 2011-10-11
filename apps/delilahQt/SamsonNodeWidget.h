#include <QtGui>
#include <sstream>


#include "au/Table.h"

#include "ui_SamsonNodeWidget.h"


namespace au
{
    class TreeItem;
}

class SamsonNodeWidget : public QWidget , public Ui_SamsonNodeWidget
{
    
    QStandardItemModel *myModel;
    
    Q_OBJECT
    
public:
    
    SamsonNodeWidget(QWidget *parent = 0);

    void setTitle( std::string _title );
    void update( au::TreeItem *treeItem );    
    
    private slots:
    
    
    
private:
    

    
};

