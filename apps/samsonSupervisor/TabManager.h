#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include <QDialog>

class QTabWidget;



class TabManager : public QWidget
{
	Q_OBJECT

public:
	TabManager(QWidget* window, QWidget *parent = 0);

private:
	QTabWidget*       tabWidget;
};

#endif
