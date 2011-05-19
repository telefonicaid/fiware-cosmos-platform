/* ****************************************************************************
*
* FILE                     QsiInputDialog.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 19 2011
*
*/
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
	int inputs = 0;
	InputDialog* idialog = (InputDialog*) vP;

	if (idialog->callback)
	{
		for (int ix = 0; idialog->input[ix] != NULL; ix++)
			++inputs;

		char** texts   = (char**) calloc(inputs, sizeof(char*));
		char** results = (char**) calloc(inputs, sizeof(char*));

		for (int ix = 0; ix < inputs; ix++)
		{
			texts[ix]   = strdup(idialog->input[ix]->title->getText());
			results[ix] = strdup(idialog->input[ix]->input->getText());
		}

		idialog->callback(texts, results);
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
	int inputs = 0;
	int tx, ty, tw, th;
	int wMax = 0;

	output = _output;
	
    for (ix = 0; inputTitle[ix] != NULL; ix++)
		++inputs;
	input = (InputLine**) calloc(inputs, sizeof(InputLine*));

	for (ix = 0; inputTitle[ix] != NULL; ix++)
	{
		input[ix] = new InputLine(winBox, inputTitle[ix], (const char*) NULL, (const char*) NULL, 20, 20);
		input[ix]->setZValue(0.71);

		input[ix]->title->geometry(&tx, &ty, &tw, &th);
		wMax = MAX(wMax, tw);
	}
	
	LM_M(("Moving all titles and inputs to be aligned in X"));
	int bx, by;

	absPos(&bx, &by);
	LM_M(("Absolute position for '%s %s' is { %d, %d }", typeName(), name, bx, by));
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

	doneButton = (Block*) winBox->buttonAdd("InputDialogButton", buttonText, 0, 0, -1, -1, pressed, this);
	doneButton->align(Alignment::South, input[ix - 1], 30);
	
	callback = _callback;
}



/* ****************************************************************************
*
* InputDialog Destructor - 
*/
InputDialog::~InputDialog()
{
	for (int ix = 0; input[ix] != NULL; ix++)
		delete input[ix];
	delete doneButton;
}

}
