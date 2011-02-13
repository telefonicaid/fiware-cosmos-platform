#ifndef CONFIG_TAB_H
#define CONFIG_TAB_H

/* ****************************************************************************
*
* FILE                     ConfigTab.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 02 2011
*
*/
#include <QObject>
#include <QWidget>

class QCheckBox;



/* ****************************************************************************
*
* ConfigTab -
*/
class ConfigTab : public QWidget
{
	Q_OBJECT

public:
	enum TypeCheck
	{
		On,
		Off,
		Left,
		Popup
	};

public:
	ConfigTab(QWidget *parent = 0);

	TypeCheck   typeCheck;
	QCheckBox*  autoConfigCBox;

private slots:
	void typeCheckOn(void);
	void typeCheckOff(void);
	void typeCheckLeft(void);
	void typeCheckPopup(void);

	void logViewClear(void);
	void logViewFit(void);
};

#endif
