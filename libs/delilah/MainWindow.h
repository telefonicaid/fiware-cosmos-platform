#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QAction>
#include <QSvgRenderer>

#include "ui_MainWindow.h"
#include "QueueItem.h"
#include "ProcessScene.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
	void createTab(QString name="");
	void removeTab(int);
	void setToolForAction(QAction* action);

	void addQueue(const QPoint &pos);
	void showAvailableQueues();
	void createTXTQueue();
	void createKVQueue();

signals:
	void toolChanged(int);

private:
    Ui::MainWindowClass ui;
    QActionGroup* tool_group;
};

#endif // MAINWINDOW_H
