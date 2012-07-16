#ifndef QSI_INPUT_H
#define QSI_INPUT_H

/* ****************************************************************************
*
* FILE                     QsiInputLine.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include "QsiBox.h"                  // Box
#include "QsiInputReturnFunction.h"  // InputReturnFunction



namespace Qsi
{



/* ****************************************************************************
*
* - 
*/
class Block;



/* ****************************************************************************
*
* InputLine - 
*/
class InputLine : public Box
{
public:
	Block*               title;
	Block*               input;
	InputReturnFunction  callback;

private:
	Block*               button;

public:
	InputLine
	(
		Box*                   owner,
		const char*            _title,
		const char*            _initInput,
		const char*            _buttonText,
		int                    x,
		int                    y,
		int                    x2      = -1,
		int                    x3      = -1,
		InputReturnFunction    onClick = NULL
	);
	~InputLine();

	void setZValue(float z);
};

}

#endif
