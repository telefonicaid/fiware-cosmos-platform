/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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
