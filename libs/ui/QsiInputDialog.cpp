/* ****************************************************************************
*
* FILE                     QsiInputDialog.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 19 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBlock.h"           // Block
#include "QsiDialog.h"          // Dialog
#include "QsiInputLine.h"       // InputLine
#include "QsiInputDialog.h"     // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* pressed - 
*/
static void pressed(Block* wbP, void* vP)
{
	InputDialog* idialog = (InputDialog*) vP;

	if (idialog->callback)
	{
		char** texts   = (char**) calloc(idialog->inputs + 1, sizeof(char*));
		char** results = (char**) calloc(idialog->inputs + 1, sizeof(char*));

		LM_T(LmtInputDialog, ("We have %d inputs", idialog->inputs));

		for (int ix = 0; ix < idialog->inputs; ix++)
		{
			texts[ix] = strdup(idialog->input[ix]->title->getText());
			results[ix] = strdup(idialog->input[ix]->input->getText());

			LM_T(LmtInputDialog, ("%02d: %s: %s", ix, texts[ix], results[ix]));
		}

		if (idialog->callback != NULL)
		{
			LM_T(LmtInputDialog, ("Calling callback at %p", idialog->callback));
			idialog->callback(texts, results);
		}
		else
			LM_T(LmtInputDialog, ("NULL callback ..."));

		delete idialog;
	}
}



/* ****************************************************************************
*
* InputDialog Constructor - 
*/
InputDialog::InputDialog(Manager* _manager, const char* _title, char* inputTitle[], char** _output, const char* buttonText, bool modal, InputReturnFunction _callback) :
Dialog(_manager, _title, modal)
{
	int ix;
	int tx, ty, tw, th;
	int wMax = 0;

	output = _output;
	
	inputs = 0;
    for (ix = 0; inputTitle[ix] != NULL; ix++)
		++inputs;
	input = (InputLine**) calloc(inputs, sizeof(InputLine*));

	for (ix = 0; inputTitle[ix] != NULL; ix++)
	{
		LM_T(LmtInputDialog, ("Creating InputLine %d", ix));
		input[ix] = new InputLine(winBox, inputTitle[ix], (const char*) NULL, (const char*) NULL, 20, 20);
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
		
		input[ix]->title->moveRelative(20, 40 + ix * (th + 10));
		
		point = input[ix]->title->gItemP->pos();
		gx = point.x();
		gy = point.y();
		
		input[ix]->input->moveAbsolute(gx + wMax + 30, gy);
	}
	
	int buttonHeight = 50;
	doneButton = (Block*) winBox->buttonAdd("InputDialogButton", buttonText, 
											borderWidth,
											winBox->height() - shadowY - buttonHeight - 2 * borderWidth,
											winBox->width() - 2 * borderWidth - shadowX,
											buttonHeight,
											pressed, this);
	doneButton->setZValue(0.71);

	LM_T(LmtInputDialog, ("doneButton width: %d, text: '%s'", winBox->width(), buttonText));
	callback = _callback;

	LM_M(("Showing QsiInputDialog::winBox"));
	winBox->qsiShow("QsiInputDialog Made", true);
}



/* ****************************************************************************
*
* InputDialog Destructor - 
*/
InputDialog::~InputDialog()
{
	LM_M(("deleting %d inputs", inputs));
	for (int ix = 0; ix < inputs; ix++)
	{
		LM_M(("deleting input %d", ix));
		delete input[ix];
		input[ix] = NULL;
	}

	if (doneButton)
	{
		delete doneButton;
		doneButton = NULL;
	}
}

}
