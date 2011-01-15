#ifndef DELILAH_TAB_H
#define DELILAH_TAB_H

#include <QObject>
#include <QWidget>



/* ****************************************************************************
*
* DelilahTab -
*/
class DelilahTab : public QWidget
{
	Q_OBJECT

public:
	DelilahTab(const char* name, QWidget *parent = 0);
};

#endif
