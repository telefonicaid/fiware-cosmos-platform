#ifndef BUTTON_ACTIONS_H
#define BUTTON_ACTIONS_H

/* ****************************************************************************
*
* FILE                     ButtonActions.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 13 2011
*
*/
#include <QObject>
#include <QPushButton>



/* ****************************************************************************
*
* ButtonActions - 
*/
class ButtonActions : public QWidget
{
	Q_OBJECT

public:
	ButtonActions(QWidget* parent = 0);

private slots:
    void connect();
    void start();
    void quit();

private:
	QPushButton* quitButton;
	QPushButton* connectButton;
	QPushButton* startButton;
};

#endif
