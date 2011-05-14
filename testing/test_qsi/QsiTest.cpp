/* ****************************************************************************
*
* FILE                     QsiTest.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 12 2011
*
*/
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>

#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs
#include "../../libs/ui/traceLevels.h"

#include "QsiAlignment.h"       // Qsi::Alignment
#include "QsiManager.h"         // QsiManager
#include "QsiBox.h"             // QsiBox
#include "QsiBlock.h"           // QsiBlock



/* ****************************************************************************
*
* Window geometry
*/
#define MAIN_WIN_WIDTH     1400
#define MAIN_WIN_HEIGHT     800



/* ****************************************************************************
*
* Option variables
*/
bool noPopups;



/* ****************************************************************************
*
* Parse arguments
*/
PaArgument paArgs[] =
{
	{ "-noPopups",    &noPopups,       "NO_POPUPS",     PaBool,    PaOpt, false, false,   true,  "No popup windows", PA_REST  },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* qsi widgets - 
*/
QWidget*          mainWindow               = NULL;
QDesktopWidget*   desktop                  = NULL;

Qsi::QsiManager*  qsiManager = NULL;

Qsi::QsiBox*      inputBox;
Qsi::QsiBlock*    inputP;
Qsi::QsiBlock*    inputOkButton;

Qsi::QsiBlock*    tableImageP;
Qsi::QsiBlock*    bigTableImageP;
Qsi::QsiBlock*    user1;
Qsi::QsiBlock*    user2;

Qsi::QsiBlock*    textButton;
Qsi::QsiBlock*    moveButton;
Qsi::QsiBlock*    expandButton;
Qsi::QsiBlock*    compressButton;
Qsi::QsiBlock*    fontButton;
Qsi::QsiBlock*    colorButton;
Qsi::QsiBlock*    boldButton;
Qsi::QsiBlock*    italicButton;

Qsi::QsiBlock*    alignSouthButton;
Qsi::QsiBlock*    alignNorthButton;
Qsi::QsiBlock*    alignWestButton;
Qsi::QsiBlock*    alignEastButton;



/* ****************************************************************************
*
* textClicked - 
*/
void textClicked(Qsi::QsiBlock* qbP, void* param)
{
	LM_T(LmtMouse, ("Text '%s' Clicked. Param: %p", qbP->name, param));
	if (qbP->isExpanded())
		qbP->hideOthers();
	else
		qbP->showOthers();

	LM_W(("Expand/Compress doesn't exist anymore ..."));
}



/* ****************************************************************************
*
* buttonClicked - 
*/
void buttonClicked(Qsi::QsiBlock* qbP, void* param)
{
	QPushButton* button;
	static int   r       = 0x60;
	static int   g       = 0x80;
	static int   b       = 0xA0;
	static int   a       = 0xFF;
	static bool  bold    = true;
	static bool  italic  = false;
	static int   counter = 0;

	button = qbP->w.button;

	LM_T(LmtMouse, ("Button '%s' Clicked. Param at %p", qbP->name, param));

	if (qbP == textButton)
	{
		++counter;
		if ((counter % 3) == 1)
			user1->setText("Ken Zangelin Jansson");
		else if ((counter % 3) == 2)
			user1->setText("kz");
		else
			user1->setText("Ken Zangelin");
	}
	else if (qbP == moveButton)
		user1->moveRelative(20, 0);
	else if (qbP == expandButton)
		LM_W(("Expand doesn't exist anymore"));
	else if (qbP == compressButton)
		LM_W(("Compress doesn't exist anymore"));
	else if (qbP == colorButton)
	{
		user1->setColor(r, g, b, a);
		r = (r + 10) % 256;
		g = (g + 15) % 256;
		b = (b + 20) % 256;
		a = (a + 10) % 256;
	}
	else if (qbP == fontButton)
		user1->setFont(QFont("Helvetica"));
	else if (qbP == boldButton)
	{
		bold = (bold == true)? false : true;
		user1->setBold(bold);
	}
	else if (qbP == italicButton)
	{
		italic = (italic == true)? false : true;
		user1->setItalic(italic);
	}
	else if (qbP == alignSouthButton)
		user2->getOwner()->align(Qsi::Alignment::South, user1->getOwner(), 20);
	else if (qbP == alignNorthButton)
		user2->getOwner()->align(Qsi::Alignment::North, user1->getOwner(), 20);
	else if (qbP == alignWestButton)
		user2->getOwner()->align(Qsi::Alignment::West, user1->getOwner(), 20);
	else if (qbP == alignEastButton)
		user2->getOwner()->align(Qsi::Alignment::East, user1->getOwner(), 20);
	else if (qbP == inputOkButton)
		LM_T(LmtInput, ("Input Test: '%s'", inputP->getText()));
	else
		LM_W(("No button found"));
}



void userEdit(Qsi::QsiBlock* qbP, void* param)
{
	LM_W(("Edit user '%s', param: %p - Not Implemented", qbP->name, param));
}



void userRemove(Qsi::QsiBlock* qbP, void* param)
{
	LM_W(("Removing user '%s', param: %p Not Implemented", qbP->name, param));
}

void userColor(Qsi::QsiBlock* qbP, void* param)
{
	LM_W(("Changing color for user '%s', param: %p - Not Implemented", qbP->name, param));
}



static void qsiSetup(QWidget* mainWindow)
{
	QVBoxLayout*    layout = new QVBoxLayout();
	Qsi::QsiBlock*  numberImageP;
	unsigned int    ix;
	const char*     props1[] =
	{
		"Padre",
		"45 years",
		"Computer Engineer",
		"Telefonica I+D",
		"Swedish",
		"English",
		"Spanish"
	};
	const char* props2[] =
	{
		"Hijo",
		"12 years",
		"Student",
		"Baix a Mar",
		"Spanish",
		"Catalan",
		"Swedish"
	};
	
	mainWindow->setLayout(layout);

	qsiManager = new Qsi::QsiManager(layout, "./TestImages", "Background.png", 1920, 1080);


	Qsi::QsiBox*   mainBox   = qsiManager->box;
	Qsi::QsiBox*   userBox1  = (Qsi::QsiBox*) mainBox->boxAdd("userBox1", 200, 90);
	Qsi::QsiBox*   userBox2  = (Qsi::QsiBox*) mainBox->boxAdd("userBox2", 500, 190);

	//
	// User 1
	//
	user1   = (Qsi::QsiBlock*) userBox1->textAdd("User1", "Ken Zangelin", 0, 0);
	user1->setBold(true);
	user1->setMovable(true);
	user1->setBoxMove(true);

	for (ix = 0; ix < sizeof(props1) / sizeof(props1[0]); ix++)
		userBox1->textAdd(props1[ix], props1[ix],   10, (ix + 1) * 20 + 5);

#if 0
	int x,y,w,h;
	userBox1->geometry(&x,&y,&w,&h);
	LM_M(("Geometry for %s: { %d, %d } %d x %d", userBox1->name, x,y,w,h));
	userBox1->lineAdd("delimiter1", 0, y, w, y);
#endif

	user1->menuAdd("Edit User",    userEdit,   user1);
	user1->menuAdd("Remove User",  userRemove, user1);
	user1->menuAdd("Color Change", userColor,  user1);

	qsiManager->siConnect(user1, textClicked, NULL);


	//
	// User 2
	//
	user2 = (Qsi::QsiBlock*) userBox2->textAdd("User2", "Alex Zangelin Calvo", 0, 0);
	user2->setBold(true);
	user2->setMovable(true);
	user2->setBoxMove(true);

	for (ix = 0; ix < sizeof(props2) / sizeof(props2[0]); ix++)
		userBox2->textAdd(props2[ix], props2[ix],   10, (ix + 1) * 20 + 5);

#if 0
	userBox2->geometry(&x,&y,&w,&h);
	LM_M(("Geometry for %s: { %d, %d } %d x %d", userBox2->name, x,y,w,h));
	userBox2->lineAdd("delimiter2", 0, y, w, y);
#endif

	user2->menuAdd("Edit User",    userEdit,   user2);
	user2->menuAdd("Remove User",  userRemove, user2);
	user2->menuAdd("Color Change", userColor,  user2);

	qsiManager->siConnect(user2, textClicked, NULL);



	//
	// Buttons
	//
	Qsi::QsiBox* buttonBox = (Qsi::QsiBox*) mainBox->boxAdd("buttonBox", 10, 10);

	textButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("TextButton",     "Change Text",          0,  10, 150, 25);
	moveButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("MoveButton",     "Move User Group",      0,  40, 150, 25);
	expandButton       = (Qsi::QsiBlock*) buttonBox->buttonAdd("ExpandButton",   "Expand User Group",    0,  70, 150, 25);
	compressButton     = (Qsi::QsiBlock*) buttonBox->buttonAdd("CompressButton", "Compress User Group",  0, 100, 150, 25);
	fontButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("FontButton",     "Font",                 0, 130, 150, 25);
	colorButton        = (Qsi::QsiBlock*) buttonBox->buttonAdd("ColorButton",    "Color",                0, 160, 150, 25);
	boldButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("BoldButton",     "Bold",                 0, 190, 150, 25);
	italicButton       = (Qsi::QsiBlock*) buttonBox->buttonAdd("ItalicButton",   "Italic",               0, 220, 150, 25);

	alignSouthButton   = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignSouth",     "Align South",          0, 270, 150, 25);
	alignNorthButton   = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignNorth",     "Align North",          0, 300, 150, 25);
	alignWestButton    = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignWest",      "Align To West",        0, 330, 150, 25);
	alignEastButton    = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignEast",      "Align To East",        0, 360, 150, 25);

	textButton->setMovable(true);
	textButton->setBoxMove(true);

	
	//
	// Input
	//
	Qsi::QsiBox* inputBox = (Qsi::QsiBox*) mainBox->boxAdd("inputBox", 10, 650);

	inputP             = (Qsi::QsiBlock*) inputBox->inputAdd("Input1",          "Input1",              0, 0, 200, 50);
	inputOkButton      = (Qsi::QsiBlock*) inputBox->buttonAdd("Input1OK",       "OK",                  0, 0,  50, 25);

	inputP->setMovable(true);
	inputP->setBoxMove(true);

	inputOkButton->setMovable(true);
	inputOkButton->setBoxMove(true);
	inputOkButton->align(Qsi::Alignment::East, inputP, 20);



	//
	// Small table
	//
	Qsi::QsiBox* smallTableBox = (Qsi::QsiBox*) mainBox->boxAdd("smallTableBox", 700, 150);

	tableImageP      = (Qsi::QsiBlock*) smallTableBox->imageAdd("Table",  "Table.png", 0, 0, -1, -1);
	numberImageP     = (Qsi::QsiBlock*) smallTableBox->imageAdd("Number", "seven.png", 50, 40, 20, 20);

	numberImageP->align(Qsi::Alignment::Center, tableImageP, 0);
	numberImageP->moveRelative(0, -15);
	tableImageP->setMovable(true);
	tableImageP->setBoxMove(true);


	//
	// Big table
	//
	bigTableImageP   = (Qsi::QsiBlock*) mainBox->imageAdd("Big Table",  "Table.png", 600, 400, 300, 300);

	tableImageP->setMovable(true);
	bigTableImageP->setMovable(true);



	//
	// Connect callbacks for buttons
	//
	qsiManager->siConnect(textButton,         buttonClicked, user1);
	qsiManager->siConnect(moveButton,         buttonClicked, NULL);
	qsiManager->siConnect(expandButton,       buttonClicked, NULL);
	qsiManager->siConnect(compressButton,     buttonClicked, NULL);
	qsiManager->siConnect(colorButton,        buttonClicked, NULL);
	qsiManager->siConnect(fontButton,         buttonClicked, NULL);
	qsiManager->siConnect(boldButton,         buttonClicked, NULL);
	qsiManager->siConnect(italicButton,       buttonClicked, NULL);
	qsiManager->siConnect(alignSouthButton,   buttonClicked, NULL);
	qsiManager->siConnect(alignNorthButton,   buttonClicked, NULL);
	qsiManager->siConnect(alignWestButton,    buttonClicked, NULL);
	qsiManager->siConnect(alignEastButton,    buttonClicked, NULL);
	qsiManager->siConnect(inputOkButton,      buttonClicked, NULL);
}



/* ****************************************************************************
*
* mainWinCreate - 
*/
static void mainWinCreate()
{
	int   screenWidth;
	int   screenHeight;
	int   x;
	int   y;

	desktop    = QApplication::desktop();
	mainWindow = new QWidget();

	// Window Geometry
	screenWidth  = desktop->width();
	screenHeight = desktop->height();

	x = (screenWidth  - MAIN_WIN_WIDTH)  / 2;
	y = (screenHeight - MAIN_WIN_HEIGHT) / 2;

	mainWindow->resize(MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT);
	mainWindow->move(x, y);
	mainWindow->setWindowTitle("Qsi Test");

	qsiSetup(mainWindow);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
	QApplication   app(argC, (char**) argV);

	paConfig("prefix",                        (void*) "QSI_TEST_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC:FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_D(("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_D(("  %02d: '%s'", ix, argV[ix]));

	mainWinCreate();
	mainWindow->show();

    qApp->exec();
}

