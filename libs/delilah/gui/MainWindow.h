#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QAction>
#include <QSvgRenderer>

#include "ui_MainWindow.h"
#include "QueueItem.h"
#include "WorkspaceScene.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
//	void createTab(QString name="");
	void createNewWorkspace(QString name="");
	void removeTab(int);
	void setToolForAction(QAction* action);

signals:
	void toolChanged(int);

private:
    Ui::MainWindowClass ui;
    QActionGroup* tool_group;
    unsigned int tab_id;		// Counter of the tabs (used for creating tab's default name)
};

#endif // MAINWINDOW_H
