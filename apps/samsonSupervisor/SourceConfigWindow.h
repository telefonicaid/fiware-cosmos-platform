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
class QLabel;
class QPushButton;
class QSpinBox;



/* ****************************************************************************
*
* SourceConfigWindow - 
*/
class SourceConfigWindow : public QDialog
{
	Q_OBJECT

public:
	SourceConfigWindow(DelilahSource* source);

	void            sourceFileShow(bool show);
	void            fakeShow(bool show);

	bool            faked;
	int             fakeSize;
	DelilahSource*  source;
	QLineEdit*      displayNameInput;
	QComboBox*      outTypeCombo;

	QLineEdit*      sourceFileNameInput;
	QLabel*         sourceFileNameLabel;
	QPushButton*    sourceFileNameBrowseButton;
	QLabel*         fakeLabel;
	QPushButton*    fakeButton;
	QPushButton*    sourceFileButton;
	QSpinBox*       fakeSizeSpinBox;

private slots:
	void die(void);
	void ok(void);
	void save(void);
	void apply(void);
	void cancel(void);

	void browse(void);
	void fake(void);
	void unfake(void);
};

#endif
