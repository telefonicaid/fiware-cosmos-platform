/* ****************************************************************************
*
* FILE                     QsiInputLine.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 17 2011
*
*/
#include "QsiBox.h"             // Box
#include "QsiBlock.h"           // Block
#include "QsiInputLine.h"       // Own interface



namespace Qsi
{



/* ****************************************************************************
*
* buttonCallback - 
*/
static void buttonCallback(Block* button, void* vP)
{
	InputLine*   inputLine = (InputLine*) vP;
	const char*  titleV[2] = 
	{
		strdup(inputLine->title->getText()),
		NULL
	};
	const char*  resultV[2] = 
	{
		strdup(inputLine->input->getText()),
		NULL
	};

	if (inputLine->callback)
		inputLine->callback(titleV, resultV);

	delete inputLine;
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
	title   = (Block*) textAdd("InputLineTitle",    _title,      0, 0);
	input   = (Block*) inputAdd("InputLineInput",   _initInput,  0, 0);
	button  = (Block*) buttonAdd("InputLineButton", _buttonText, 0, 0, -1, -1, buttonCallback, this);

	input->align(Alignment::East, title, inputMargin);
	button->align(Alignment::East, input, buttonMargin);

	callback = onClick;

	owner->add(this);

	title->boxMoveSet(true);
	input->boxMoveSet(true);
	button->boxMoveSet(true);
}



/* ****************************************************************************
*
* Destructor - 
*/
InputLine::~InputLine()
{
	owner->remove(this, false);
	delete title;
	delete input;
	delete button;
}

}
