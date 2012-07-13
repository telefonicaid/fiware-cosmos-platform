/* ****************************************************************************
*
* FILE                     QsiInputLine.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include "logMsg/logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*

#include "QsiBox.h"             // Box
#include "QsiBlock.h"           // Block
#include "QsiInputLine.h"       // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* buttonCallback - 
*/
static void buttonCallback(Block* button, void* vP, const char* nada)
{
	InputLine*  inputLine = (InputLine*) vP;
	char*       titleV[2] = 
	{
		strdup(inputLine->title->getText()),
		NULL
	};
	char* resultV[2] = 
	{
		strdup(inputLine->input->getText()),
		NULL
	};

	if (inputLine->callback)
		inputLine->callback(titleV, resultV);

	delete inputLine;
	nada = NULL;
}



/* ****************************************************************************
*
* InputLine - 
*/
InputLine::InputLine
(
	Box*                   owner,
	const char*            _title,
	const char*            _initInput,
	const char*            _buttonText,
	int                    x,
	int                    y,
	int                    inputMargin,
	int                    buttonMargin,
	InputReturnFunction    onClick
) : Box(owner->manager, owner, _title, x, y)
{
	int tx, ty, tw, th;
	int ix, iy, iw, ih;

	this->typeSet(InputLineItem);

	title   = (Block*) textAdd("InputLineTitle",    _title,      0, 0);
	input   = (Block*) inputAdd("InputLineInput",   _initInput,  0, 0);
	button  = NULL;

	input->align(Alignment::East, title, inputMargin);
	callback = onClick;

	title->geometry(&tx, &ty, &tw, &th);
	input->geometry(&ix, &iy, &iw, &ih);

	title->moveRelative(0, (ih - th) / 2);

	owner->add(this);

	title->boxMoveSet(true);
	input->boxMoveSet(true);

	if (_buttonText != NULL)
	{
		button  = (Block*) buttonAdd("InputLineButton", _buttonText, 0, 0, -1, -1, buttonCallback, this);

		LM_T(LmtInputLine, ("Aligning InputLineButton to 'input' with margin %d", buttonMargin));
		button->align(Alignment::East, input, buttonMargin);
		button->boxMoveSet(true);
	}
}



/* ****************************************************************************
*
* Destructor - 
*/
InputLine::~InputLine()
{
	LM_T(LmtDelete, ("In destructor"));

	owner->remove(this, false);
	delete title;
	delete input;

	LM_T(LmtDelete, ("Deleting button?"));
	if (button)
		delete button;

	LM_T(LmtDelete, ("From destructor"));
}



/* ****************************************************************************
*
* setZValue - 
*/
void InputLine::setZValue(float z)
{
	title->setZValue(z);
	input->setZValue(z);

	if (button)
		button->setZValue(z);
}

}
