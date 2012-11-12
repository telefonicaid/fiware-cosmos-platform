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
/* ****************************************************************************
*
* FILE                     QsiInputDialog.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 19 2011
*
*/
#include "logMsg/logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBlock.h"           // Block
#include "QsiDialog.h"          // Dialog
#include "QsiInputLine.h"       // InputLine
#include "QsiInputDialog.h"     // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* donePressed - 
*/
static void donePressed(Block* wbP, void* vP, const char* nada)
{
	InputDialog* idialog = (InputDialog*) vP;
	int          ix;

	LM_T(LmtInputDialog, ("****************** DONE PRESSED !!!"));

	if (idialog->callback)
	{
		char** texts   = (char**) calloc(idialog->inputs + 2, sizeof(char*));
		char** results = (char**) calloc(idialog->inputs + 2, sizeof(char*));

		LM_T(LmtInputDialog, ("We have %d inputs", idialog->inputs));

		for (ix = 0; ix < idialog->inputs; ix++)
		{
			texts[ix] = strdup(idialog->input[ix]->title->getText());
			results[ix] = strdup(idialog->input[ix]->input->getText());

			LM_T(LmtInputDialog, ("%02d: %s: %s", ix, texts[ix], results[ix]));
		}

		LM_T(LmtInputDialog, ("Combo ..."));
		texts[ix] = strdup("User Type");
		if (idialog->combo != NULL)
		{
			QString content = idialog->combo->w.combo->currentText();

			if (content != "")
				results[ix] = strdup(content.toStdString().c_str());
			else
				results[ix] = NULL;
		}
		else
			results[ix] = NULL;
		LM_T(LmtInputDialog, ("Combo Result: '%s'", results[ix]));

		texts[ix + 1]   = NULL;
		results[ix + 1] = NULL;

		if (idialog->callback != NULL)
		{
			LM_T(LmtInputDialog, ("Calling callback at %p", idialog->callback));
			idialog->callback(texts, results);
		}
		else
			LM_T(LmtInputDialog, ("NULL callback ..."));

		idialog->manager->ungrab(idialog->winBox);
		idialog->manager->box->remove(idialog, false);
		delete idialog;
	}

	nada = NULL;
}



/* ****************************************************************************
*
* cancel - 
*/
static void cancel(Block* qbP, void* param, const char* nada)
{
	InputDialog* idialog = (InputDialog*) param;

	LM_T(LmtInputDialog, ("****************** WINBOX %p PRESSED - CANCEL !!!", idialog));

	if (idialog == NULL)
	{
		LM_W(("NULL InputDialog - what the fuck happened ?!!!"));
		return;
	}

	idialog->manager->ungrab(idialog->winBox);
	idialog->manager->box->remove(idialog, false);
	delete idialog;
	nada = NULL;
}


#if 0
/* ****************************************************************************
*
* lineCreator - 
*
*   I will need a 'TextCombo' Box first ...
*
*   InputLine | TEXT | Default Value | button text (optional)
*   Combo     | TEXT | Value1 | Value2 | Value3 ...
*
*   Line      | WIDTH
*   Rectangle | WIDTH | HEIGHT | FG Color | BG color
*   Image     | PATH
*   Button    | button text
*   Input     | TEXT | Default Value
*
*/
Base* lineCreator(char* info)
{
	char* objName;
	char* 
}
#endif



/* ****************************************************************************
*
* InputDialog Constructor - 
*/
InputDialog::InputDialog
(
	Manager*             _manager,
	const char*          _title,
	char*                inputTitle[],
	char**               _output,
	const char*          buttonText,
	const char**         comboContent,
	bool                 modal,
	InputReturnFunction  _callback
) : Dialog(_manager, _title, modal, false)
{
	int ix;
	int tx, ty, tw, th;
	int wMax = 0;

	this->typeSet(InputDialogItem);

	output   = _output;
	callback = NULL;

	inputs = 0;
    for (ix = 0; inputTitle[ix] != NULL; ix++)
		++inputs;
	input = (InputLine**) calloc(inputs, sizeof(InputLine*));

	LM_T(LmtInputLine, ("Creating %d InputLines", inputs));
	for (ix = 0; inputTitle[ix] != NULL; ix++)
	{
		char* txt;
		char* value;

		LM_T(LmtInputLine, ("inputTitle[%d]: '%s'", ix, inputTitle[ix]));
		txt   = inputTitle[ix];
		value = strstr(inputTitle[ix], "|");
		if (value != NULL)
		{
			*value = 0;
			value++;
			if (*value == 0)
				value = NULL;

			LM_T(LmtInputLine, ("Found '|': txt: '%s', value: '%s'", txt, value));
		}

		LM_T(LmtInputDialog, ("Creating InputLine %d", ix));
		input[ix] = new InputLine(winBox, txt, value, (const char*) NULL, 20, 20);
		input[ix]->setZValue(0.71);
		
		input[ix]->title->geometry(&tx, &ty, &tw, &th);
		wMax = MAX(wMax, tw);
	}
	
	LM_T(LmtInputDialog, ("Moving all titles and inputs to be aligned in X"));
	int bx, by;

	absPos(&bx, &by);
	LM_T(LmtInputDialog, ("Absolute position for '%s %s' is { %d, %d }", typeName(), name, bx, by));
    for (ix = 0; inputTitle[ix] != NULL; ix++)
	{
		QPointF point;
		int gx, gy;
		
		input[ix]->title->moveRelative(20, 60 + ix * (th + 10));
		
		point = input[ix]->title->gItemP->pos();
		gx = point.x();
		gy = point.y();
		
		input[ix]->input->moveAbsolute(gx + wMax + 30, gy);

		input[ix]->title->setMovable(false);
		input[ix]->input->setMovable(false);
	}
	
	if (comboContent != NULL)
	{
		LM_T(LmtCombo, ("Adding combo box"));
		combo = (Block*) winBox->comboAdd("userTypeCombo", comboContent, wMax + 70, 40);
		combo->setZValue(0.72);
		LM_T(LmtCombo, ("Combo box added"));
	}
	else
		combo = NULL;
	

	int buttonHeight = 50;
	doneButton = (Block*) winBox->buttonAdd("InputDialogButton", buttonText, 
											borderWidth,
											winBox->height() - shadowY - buttonHeight - 2 * borderWidth,
											winBox->width() - 2 * borderWidth - shadowX,
											buttonHeight,
											donePressed, this);
	doneButton->setZValue(0.71);
	doneButton->setMovable(false);

	LM_T(LmtInputDialog, ("doneButton width: %d, text: '%s'", winBox->width(), buttonText));
	callback = _callback;

	winBox->qsiShow("QsiInputDialog Made", true);

	//
	// Connecting dialog window background to CANCEL, using the 'cancel' function
	//
	manager->siConnect(win, cancel, this);

	if (modal)
		manager->grab(winBox);
}



/* ****************************************************************************
*
* InputDialog Destructor - 
*/
InputDialog::~InputDialog()
{
	LM_T(LmtDelete, ("deleting %d inputs", inputs));
	for (int ix = 0; ix < inputs; ix++)
	{
		LM_T(LmtDelete, ("deleting input %d", ix));
		delete input[ix];
		input[ix] = NULL;
	}

	if (doneButton)
	{
		delete doneButton;
		doneButton = NULL;
	}

	if (combo != NULL)
	{
		delete combo;
		combo = NULL;
	}
}

}
