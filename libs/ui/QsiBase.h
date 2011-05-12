#ifndef QSI_BASE_H
#define QSI_BASE_H

/* ****************************************************************************
*
* FILE                     QsiBase.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 09 2011
*
*/
#include <QObject>


namespace Qsi
{



/* ****************************************************************************
*
* QsiBase - 
*/
class QsiBase : public QObject
{
	friend class QsiBlock;

private:
	int                x;
	int                y;
	int                xpos;
	int                ypos;

public:
	char*              name;

	QsiBase(const char* _name, int _x, int _y)
	{
		name   = strdup(_name);
		xpos   = _x;
		ypos   = _y;
		x      = _x;
		y      = _y;
	};

	~QsiBase()
	{
		free(name);
	};

	virtual void move(int x, int y) = 0;
	virtual void move(void)         = 0;
};

}

#endif
