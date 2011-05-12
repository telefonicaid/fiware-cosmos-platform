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

#include "QsiManager.h"         // QsiManager
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

Qsi::QsiBlock*    alignUnderButton;
Qsi::QsiBlock*    alignOverButton;
Qsi::QsiBlock*    alignToLeftButton;
Qsi::QsiBlock*    alignToRightButton;



/* ****************************************************************************
*
* textClicked - 
*/
void textClicked(Qsi::QsiBlock* siP, void* param)
{
	LM_M(("Text '%s' Clicked. Param: %p", siP->name, param));
}



/* ****************************************************************************
*
* buttonClicked - 
*/
void buttonClicked(Qsi::QsiBlock* siP, void* param)
{
	QPushButton* button;
	static int   r      = 0x60;
	static int   g      = 0x80;
	static int   b      = 0xA0;
	static int   a      = 0xFF;
	static bool  bold   = true;
	static bool  italic = false;

	button = siP->w.button;

	LM_M(("Button '%s' Clicked. Param at %p", siP->name, param));

	if (siP == textButton)
		; // user1->setText("User Name changed");
	else if (siP == moveButton)
		siP->manager->groupMove(user1, 20, 0);
	else if (siP == expandButton)
		user1->expand();
	else if (siP == compressButton)
		user1->compress();
	else if (siP == colorButton)
	{
		user1->setColor(r, g, b, a);
		r = (r + 10) % 256;
		g = (g + 15) % 256;
		b = (b + 20) % 256;
		a = (a + 10) % 256;
	}
	else if (siP == fontButton)
		user1->setFont(QFont("Helvetica"));
	else if (siP == boldButton)
	{
		bold = (bold == true)? false : true;
		user1->setBold(bold);
	}
	else if (siP == italicButton)
	{
		italic = (italic == true)? false : true;
		user1->setItalic(italic);
	}
	else if (siP == alignUnderButton)
		user2->align(Qsi::QsiBlock::Under, user1, 20);
	else if (siP == alignOverButton)
		user2->align(Qsi::QsiBlock::Over, user1, 20);
	else if (siP == alignToLeftButton)
		user2->align(Qsi::QsiBlock::ToLeft, user1, 20);
	else if (siP == alignToRightButton)
		user2->align(Qsi::QsiBlock::ToRight, user1, 20);
	else if (siP == inputOkButton)
		LM_M(("Input Test: '%s'", inputP->getText()));
	else
		LM_W(("No button found"));
}



void userEdit(Qsi::QsiBlock* siP, void* param)
{
	LM_M(("Edit user '%s', param: %p", siP->name, param));
}



void userRemove(Qsi::QsiBlock* siP, void* param)
{
	LM_M(("NOT YET Removing user '%s', param: %p", siP->name, param));
}

void userColor(Qsi::QsiBlock* siP, void* param)
{
	LM_M(("NOT YET Changing color for user '%s', param: %p", siP->name, param));
}



static void qsiSetup(QWidget* mainWindow)
{
	QVBoxLayout*    layout = new QVBoxLayout();
	Qsi::QsiBlock*  item;
	Qsi::QsiBlock*  line;
	Qsi::QsiBlock*  numberImageP;
	int             x;
	int             y;
	unsigned int    ix;
	const char*     dishes1[] =
	{
		"Zumo de Papaya",
		"Gazpacho",
		"Steak Tartar",
		"Tarta de Queso",
		"Botellin de Agua",
		"Cerveza",
		"Cafe"
	};
	const char* dishes2[] =
	{
		"Zumo de Naranja",
		"Ensalada Mixta",
		"Pato Confitado",
		"Helado de Coco",
		"Tinto de la Cata",
		"Cafe Cortado",
		"Copa de anis"
	};
	
	mainWindow->setLayout(layout);

	qsiManager = new Qsi::QsiManager(layout, "./TestImages", "Background.png", 1920, 1080);



	//
	// User 1
	//
	x       = 200;
	y       = 90;
	user1   = qsiManager->textAdd("User1", "User Number One", x, y);
	user1->setBold(true);
	user1->setMovable(true);

	for (ix = 0; ix < sizeof(dishes1) / sizeof(dishes1[0]); ix++)
	{
		item = qsiManager->textAdd(dishes1[ix], dishes1[ix],   x + 10, y + (ix + 1) * 20 + 15);
		qsiManager->group(user1, item);
	}

	line = qsiManager->lineAdd("delimiter1", x, y + (ix + 1) * 20 + 15, x + 200, y + (ix + 1) * 20 + 15);
	qsiManager->group(user1, line);

	user1->menuAdd("Edit User",    userEdit,   user1);
	user1->menuAdd("Remove User",  userRemove, user1);
	user1->menuAdd("Color Change", userColor,  user1);

	qsiManager->siConnect(user1, textClicked, NULL);



	//
	// User 2
	//
	x       = 500;
	y       = 190;
	user2   = qsiManager->textAdd("User2",       "User Number Two",   500, 190);
	user2->setBold(true);
	user2->setMovable(true);

	for (ix = 0; ix < sizeof(dishes2) / sizeof(dishes2[0]); ix++)
	{
		item = qsiManager->textAdd(dishes2[ix], dishes2[ix],   x + 10, y + (ix + 1) * 20 + 15);
		qsiManager->group(user2, item);
	}

	line = qsiManager->lineAdd("delimiter2", x, y + (ix + 1) * 20 + 15, x + 200, y + (ix + 1) * 20 + 15);
	qsiManager->group(user2, line);

	user2->menuAdd("Edit User",    userEdit,   user2);
	user2->menuAdd("Remove User",  userRemove, user2);
	user2->menuAdd("Color Change", userColor,  user2);

	qsiManager->siConnect(user2, textClicked, NULL);



	//
	// Buttons
	//
	textButton         = qsiManager->buttonAdd("TextButton",     "Change Text",          10,  10, 150, 25);
	moveButton         = qsiManager->buttonAdd("MoveButton",     "Move User Group",      10,  40, 150, 25);
	expandButton       = qsiManager->buttonAdd("ExpandButton",   "Expand User Group",    10,  70, 150, 25);
	compressButton     = qsiManager->buttonAdd("CompressButton", "Compress User Group",  10, 100, 150, 25);
	fontButton         = qsiManager->buttonAdd("FontButton",     "Font",                 10, 130, 150, 25);
	colorButton        = qsiManager->buttonAdd("ColorButton",    "Color",                10, 160, 150, 25);
	boldButton         = qsiManager->buttonAdd("BoldButton",     "Bold",                 10, 190, 150, 25);
	italicButton       = qsiManager->buttonAdd("ItalicButton",   "Italic",               10, 220, 150, 25);

	alignUnderButton   = qsiManager->buttonAdd("AlignUnder",     "Align Under",          10, 270, 150, 25);
	alignOverButton    = qsiManager->buttonAdd("AlignOver",      "Align Over",           10, 300, 150, 25);
	alignToLeftButton  = qsiManager->buttonAdd("AlignToLeft",    "Align To Left",        10, 330, 150, 25);
	alignToRightButton = qsiManager->buttonAdd("AlignToRight",   "Align To Right",       10, 360, 150, 25);

	inputP             = qsiManager->inputAdd("Input1",          "Input1",              10, 650, 200, 50);
	inputOkButton      = qsiManager->buttonAdd("Input1OK",       "OK",                  10, 650,  50, 25);

	inputP->setMovable(true);
	inputOkButton->setMovable(true);

	inputOkButton->align(Qsi::QsiBlock::ToRight, inputP, 20);
	qsiManager->group(inputP, inputOkButton);

	tableImageP      = qsiManager->imageAdd("Table",  "Tables/images/Table.png", 700, 150, -1, -1);
	numberImageP     = qsiManager->imageAdd("Number", "Tables/images/seven.png", 750, 190, 20, 20);

	qsiManager->group(tableImageP, numberImageP);
	tableImageP->setMovable(true);

	bigTableImageP   = qsiManager->imageAdd("Big Table",  "Tables/images/Table.png", 600, 400, 300, 300);

	tableImageP->setMovable(true);
	bigTableImageP->setMovable(true);

	qsiManager->siConnect(textButton,         buttonClicked, NULL);
	qsiManager->siConnect(moveButton,         buttonClicked, NULL);
	qsiManager->siConnect(expandButton,       buttonClicked, NULL);
	qsiManager->siConnect(compressButton,     buttonClicked, NULL);
	qsiManager->siConnect(colorButton,        buttonClicked, NULL);
	qsiManager->siConnect(fontButton,         buttonClicked, NULL);
	qsiManager->siConnect(boldButton,         buttonClicked, NULL);
	qsiManager->siConnect(italicButton,       buttonClicked, NULL);
	qsiManager->siConnect(alignUnderButton,   buttonClicked, NULL);
	qsiManager->siConnect(alignOverButton,    buttonClicked, NULL);
	qsiManager->siConnect(alignToLeftButton,  buttonClicked, NULL);
	qsiManager->siConnect(alignToRightButton, buttonClicked, NULL);
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

