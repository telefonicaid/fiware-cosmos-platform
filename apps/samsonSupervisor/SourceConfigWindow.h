#ifndef SOURCE_CONFIG_WINDOW_H
#define SOURCE_CONFIG_WINDOW_H

/* ****************************************************************************
*
* FILE                     SourceConfigWindow.h
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
class DelilahSource;



/* ****************************************************************************
*
* SourceConfigWindow - 
*/
class SourceConfigWindow : public QDialog
{
	Q_OBJECT

public:
	SourceConfigWindow(DelilahSource* source);

	DelilahSource* source;
	QLineEdit*     displayNameInput;
	QComboBox*     outTypeCombo;

private slots:
	void die(void);
	void ok(void);
	void save(void);
	void apply(void);
	void cancel(void);
};

#endif
