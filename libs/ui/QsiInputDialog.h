#ifndef QSI_INPUT_DIALOG_H
#define QSI_INPUT_DIALOG_H

/* ****************************************************************************
*
* FILE                     QsiInputDialog.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 19 2011
*
*/
#include "QsiDialog.h"               // Dialog
#include "QsiInputReturnFunction.h"  // InputReturnFunction



namespace Qsi
{



/* ****************************************************************************
*
* Classes
*/
class Manager;
class InputLine;
class Block;



/* ****************************************************************************
*
* InputDialog - 
*/
class InputDialog : public Dialog
{
public:
	Block*               doneButton;
	InputLine**          input;
	int                  inputs;
	char**               output;
	Block*               combo;
	InputReturnFunction  callback;

	InputDialog
	(
		Manager*     _manager,
		const char*  _title,
		char*        _inputTitle[],
		char**       _output,
		const char*  buttonText,
		const char** comboContent,
		bool         modal,
		InputReturnFunction _callback
	);

	~InputDialog();
};

}

#endif
