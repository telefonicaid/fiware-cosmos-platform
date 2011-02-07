#ifndef RESULT_CONFIG_WINDOW_H
#define RESULT_CONFIG_WINDOW_H

/* ****************************************************************************
*
* FILE                     ResultConfigWindow.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 05 2011
*
*/
#include <QObject>
#include <QDialog>

class QLineEdit;
class QComboBox;
class DelilahResult;



/* ****************************************************************************
*
* ResultConfigWindow - 
*/
class ResultConfigWindow : public QDialog
{
	Q_OBJECT

public:
	ResultConfigWindow(DelilahResult* result);

	DelilahResult*  result;
	QLineEdit*      displayNameInput;
	QComboBox*      inTypeCombo;
	QLineEdit*      resultFileNameInput;

private slots:
	void die(void);
	void ok(void);
	void save(void);
	void apply(void);
	void cancel(void);

	void browse(void);
};

#endif
