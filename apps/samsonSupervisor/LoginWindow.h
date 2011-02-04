#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

/* ****************************************************************************
*
* FILE                     LoginWindow.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 04 2011
*
*/
#include <QObject>
#include <QDialog>

class QLineEdit;



/* ****************************************************************************
*
* LoginWindow - 
*/
class LoginWindow : public QDialog
{
	Q_OBJECT

public:
	LoginWindow(void);

	QLineEdit*        userNameInput;
	QLineEdit*        passwordInput;
	int               tries;

private slots:
	void ok(void);
	void cancel(void);
};

#endif
