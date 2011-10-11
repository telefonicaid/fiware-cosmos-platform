#include <QtGui>
#include <sstream>


#include "au/Table.h"

#include "ui_SamsonQueryWidget.h"


class SamsonQueryWidget : public QWidget , public Ui_SamsonQueryWidget
{
    
    QStandardItemModel *myModel;
    
    Q_OBJECT
    
public:
    
    SamsonQueryWidget(QWidget *parent = 0);
    
    private slots:
    
    void update();
    
private:
    
    void setModel( QStandardItemModel*model );
    
    static QStandardItemModel* getExampleModel();
    static QStandardItemModel* createModel( au::DataSet* dataSet );
    
};

