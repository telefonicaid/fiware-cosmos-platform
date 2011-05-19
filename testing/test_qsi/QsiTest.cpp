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
#include "QsiManager.h"         // Qsi::Manager
#include "QsiBox.h"             // Qsi::Box
#include "QsiBlock.h"           // Qsi::Block
#include "QsiExpandList.h"      // Qsi::ExpandList
#include "QsiPopup.h"           // Qsi::Popup
#include "QsiDialog.h"          // Qsi::Dialog
#include "QsiInputLine.h"       // Qsi::InputLine



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
static bool noPopups;



/* ****************************************************************************
*
* Parse arguments
*/
static PaArgument paArgs[] =
{
	{ "-noPopups",    &noPopups,       "NO_POPUPS",     PaBool,    PaOpt, false, false,   true,  "No popup windows", PA_REST  },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* In-data for the test program
*/
static const char*     props1[] =
{
	"Padre",
	"45 years",
	"Computer Engineer",
	"Telefonica I+D",
	"Swedish",
	"English",
	"Spanish"
};

static const char* props2[] =
{
	"Hijo",
	"12 years",
	"Student",
	"Baix a Mar",
	"Spanish",
	"Catalan",
	"Swedish"
};



/* ****************************************************************************
*
* qsi widgets - 
*/
static QWidget*          mainWindow               = NULL;
static QDesktopWidget*   desktop                  = NULL;

static Qsi::Manager*     qsiManager = NULL;

static Qsi::Box*         mainBox;

static Qsi::Block*       inputP;
static Qsi::Block*       inputOkButton;

static Qsi::Block*       tableImageP;
static Qsi::Block*       bigTableImageP;

Qsi::ExpandList*         userBox1;
Qsi::ExpandList*         userBox2;
Qsi::ExpandList*         elist;

static Qsi::Block*       textButton;
static Qsi::Block*       moveButton;
static Qsi::Block*       fontButton;
static Qsi::Block*       colorButton;
static Qsi::Block*       boldButton;
static Qsi::Block*       italicButton;
static Qsi::Block*       alignSouthButton;
static Qsi::Block*       alignNorthButton;
static Qsi::Block*       alignWestButton;
static Qsi::Block*       alignEastButton;
static Qsi::Block*       alignCenterButton;
static Qsi::Block*       alignNoneButton;

static Qsi::Block*       rectangle;
static Qsi::InputLine*   inputLine;



/* ****************************************************************************
*
* fieldEdit - 
*/
static void fieldEdit(Qsi::Block* qbP, void* param)
{
	LM_W(("Edit field '%s', param: %p - Not Implemented", qbP->name, param));
}



/* ****************************************************************************
*
* userColor - 
*/
static void userColor(Qsi::Block* qbP, void* param)
{
	static int   r       = 0x60;
	static int   g       = 0x80;
	static int   b       = 0xA0;
	static int   a       = 0xFF;

	LM_W(("Changing color for user '%s', param: %p", qbP->name));

	qbP->setColor(r, g, b, a);
	r = (r + 10) % 256;
	g = (g + 15) % 256;
	b = (b + 20) % 256;
	a = (a + 10) % 256;
}



/* ****************************************************************************
*
* remove - 
*/
static void remove(Qsi::Block* qbP, void* x)
{
	Qsi::Box* box = qbP->getOwner();

	box->remove(qbP);
}



/* ****************************************************************************
*
* alignmentList - 
*/
static void alignmentList(Qsi::Block* qbP, void* x)
{
	Qsi::Box*  box   = (Qsi::Box*) x;

	box->alignShow("From Menu", true);
}



/* ****************************************************************************
*
* qsiList - 
*/
static void qsiList(Qsi::Block* qbP, void* x)
{
	Qsi::Box*  box   = (Qsi::Box*) x;

	box->qsiShow("From Menu", true);
}



/* ****************************************************************************
*
* qsiAllList - 
*/
static void qsiAllList(Qsi::Block* qbP, void* x)
{
	Qsi::Box*  box   = (Qsi::Box*) x;

	box->qsiRecursiveShow("From Menu", true);
}



/* ****************************************************************************
*
* buttonClicked - 
*/
static void buttonClicked(Qsi::Block* qbP, void* param)
{
	QPushButton*    button;
	static bool     bold    = true;
	static bool     italic  = false;
	static int      counter = 0;
	Qsi::Block*     user1   = userBox1->titleGet();

	button = qbP->w.button;

	LM_T(LmtMouse, ("Button '%s' Clicked. Param at %p", qbP->name, param));

	if (qbP == rectangle)
		delete qbP;
	if (qbP == textButton)
	{
		++counter;
		if ((counter % 3) == 1)
			userBox1->titleSet("Ken Zangelin Jansson");
		else if ((counter % 3) == 2)
			userBox1->titleSet("kz");
		else
			userBox1->titleSet("Ken Zangelin");
	}
	else if (qbP == moveButton)
		user1->getOwner()->moveRelative(20, 0);
	else if (qbP == colorButton)
		userColor(user1, NULL);
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
		userBox2->align(Qsi::Alignment::South, user1->getOwner(), 20);
	else if (qbP == alignNorthButton)
		userBox2->align(Qsi::Alignment::North, user1->getOwner(), 20);
	else if (qbP == alignWestButton)
		userBox2->align(Qsi::Alignment::West, user1->getOwner(), 20);
	else if (qbP == alignEastButton)
		userBox2->align(Qsi::Alignment::East, user1->getOwner(), 20);
	else if (qbP == alignCenterButton)
		userBox2->align(Qsi::Alignment::Center, user1->getOwner(), 20);
	else if (qbP == alignNoneButton)
		userBox2->unalign(user1->getOwner());
	else if (qbP == inputOkButton)
		LM_T(LmtInput, ("Input Test: '%s'", inputP->getText()));
	else
		LM_W(("No button found"));
}



/* ****************************************************************************
*
* compress - 
*/
static void compress(Qsi::Block* qbP, void* vP)
{
	userBox1->compress();
	userBox2->compress();
	elist->compress();


	qbP = NULL;
	vP  = NULL;
}



/* ****************************************************************************
*
* expand - 
*/
static void expand(Qsi::Block* qbP, void* vP)
{
	userBox1->expand();
	userBox2->expand();
	elist->expand();

	qbP = NULL;
	vP  = NULL;
}



/* ****************************************************************************
*
* popup - 
*/
static void popup(Qsi::Block* qbP, void* vP)
{
	new Qsi::Popup(qsiManager, "Just Testing", "This is just a test of a Popup");
	qbP = NULL;
	vP  = NULL;
}



/* ****************************************************************************
*
* dialog - 
*/
static void dialog(Qsi::Block* qbP, void* vP)
{
	new Qsi::Dialog(qsiManager, "Testing MODAL Dialog", true);
	qbP = NULL;
	vP  = NULL;
}



/* ****************************************************************************
*
* dialog2 - 
*/
static void dialog2(Qsi::Block* qbP, void* vP)
{
	new Qsi::Dialog(qsiManager, "Testing Non-MODAL Dialog", false);
	qbP = NULL;
	vP  = NULL;
}



/* ****************************************************************************
*
* elistCallback - 
*/
static void elistCallback(Qsi::Block* qbP, void* vP)
{
	char* txt = (char*) vP;

	LM_M(("ExpandList item '%s' pressed: '%s'", qbP->name, txt));

	if (strcmp(txt, "Red") == 0)
		qbP->setColor(0xFF, 0, 0);
	else if (strcmp(txt, "Green") == 0)
		qbP->setColor(0, 0xFF, 0);
	else if (strcmp(txt, "Gray") == 0)
		qbP->setColor(0x80, 0x80, 0x80);
	else if (strcmp(txt, "Remove") == 0)
		remove(qbP, vP);
}



/* ****************************************************************************
*
* userCallback - 
*/
static void userCallback(Qsi::Block* qbP, void* vP)
{
	char* txt = (char*) vP;

	LM_M(("ExpandList item '%s' pressed: '%s'", qbP->name, txt));

	if (strcmp(txt, "Edit Field") == 0)
		fieldEdit(qbP, vP);
	else if (strcmp(txt, "Remove") == 0)
		remove(qbP, vP);
	else if (strcmp(txt, "Color Change") == 0)
		userColor(qbP, vP);
}



/* ****************************************************************************
*
* inputLineFunc - 
*/
static void inputLineFunc(const char* nameV[], const char* inputV[])
{
	LM_M(("Got input from inputLine:"));
	for (int ix = 0; nameV[ix] != NULL; ix++)
		LM_M(("%s: %s", nameV[ix], inputV[ix]));
}



/* ****************************************************************************
*
* qsiSetup - 
*/
static void qsiSetup(QWidget* mainWindow)
{
	QVBoxLayout*  layout = new QVBoxLayout();

	mainWindow->setLayout(layout);

	qsiManager = new Qsi::Manager(layout, "./TestImages", "Background.png", 1920, 1080, MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT);
	mainBox    = qsiManager->box;
	


	//
	// Qsi::ExpandList test
	//
	const char* menuItem[] =
	{
		"Remove All",
		"Add item",
		"Red",
		"Green",
		"Gray",
		NULL
	};
	const char* subMenuItem[] =
	{
		"Remove",
		"Red",
		"Green",
		"Gray",
		NULL
	};

	elist = new Qsi::ExpandList(qsiManager, mainBox, "ExpandList", 0, 500, 20, 10, elistCallback);

	elist->addMember("Expansion 1", elistCallback, "1", subMenuItem);
	elist->addMember("Expansion 2", elistCallback, "2", subMenuItem);
	elist->addMember("Expansion 3", elistCallback, "3", subMenuItem);

	elist->menu(elistCallback, menuItem);

	qsiManager->menuAdd("Alignment List",   alignmentList, mainBox);
	qsiManager->menuAdd("Qsi List",         qsiList,       mainBox);
	qsiManager->menuAdd("All Qsis",         qsiAllList,    mainBox);
	qsiManager->menuAdd("Compress All",     compress,      NULL);
	qsiManager->menuAdd("Expand All",       expand,        NULL);
	qsiManager->menuAdd("Popup Test",       popup,         NULL);
	qsiManager->menuAdd("Modal Dialog",     dialog,        NULL);
	qsiManager->menuAdd("Non-modal Dialog", dialog2,       NULL);

	elist->setFrame(10);



	//
	// User 1
	//
	const char* userMenuItem[] =
	{
		"Edit Field",
		"Remove",
		"Color Change",
		NULL
	};

	userBox1 = new Qsi::ExpandList(qsiManager, mainBox, "Ken Zangelin", 300, 100, 10, 10, elistCallback, true);

	for (unsigned int ix = 0; ix < sizeof(props1) / sizeof(props1[0]); ix++)
		userBox1->addMember(props1[ix], userCallback, props1[ix], userMenuItem);

	//
	// User 2
	//
	userBox2 = new Qsi::ExpandList(qsiManager, mainBox, "Alex Zangelin Calvo", 300, 400, 10, 10, elistCallback, false);

	for (unsigned int ix = 0; ix < sizeof(props2) / sizeof(props2[0]); ix++)
		userBox2->addMember(props2[ix], userCallback, props2[ix], userMenuItem);



	//
	// Buttons
	//
	Qsi::Box* buttonBox = (Qsi::Box*) mainBox->boxAdd("buttonBox", 10, 10);

	textButton         = (Qsi::Block*) buttonBox->buttonAdd("TextButton",     "Change Text",          0,  10, 150, 25, buttonClicked, NULL);
	moveButton         = (Qsi::Block*) buttonBox->buttonAdd("MoveButton",     "Move User Group",      0,  40, 150, 25, buttonClicked, NULL);
	fontButton         = (Qsi::Block*) buttonBox->buttonAdd("FontButton",     "Font",                 0,  70, 150, 25, buttonClicked, NULL);
	colorButton        = (Qsi::Block*) buttonBox->buttonAdd("ColorButton",    "Color",                0, 100, 150, 25, buttonClicked, NULL);
	boldButton         = (Qsi::Block*) buttonBox->buttonAdd("BoldButton",     "Bold",                 0, 130, 150, 25, buttonClicked, NULL);
	italicButton       = (Qsi::Block*) buttonBox->buttonAdd("ItalicButton",   "Italic",               0, 160, 150, 25, buttonClicked, NULL);

	alignSouthButton   = (Qsi::Block*) buttonBox->buttonAdd("AlignSouth",     "Align South",          0, 200, 150, 25, buttonClicked, NULL);
	alignNorthButton   = (Qsi::Block*) buttonBox->buttonAdd("AlignNorth",     "Align North",          0, 230, 150, 25, buttonClicked, NULL);
	alignWestButton    = (Qsi::Block*) buttonBox->buttonAdd("AlignWest",      "Align To West",        0, 260, 150, 25, buttonClicked, NULL);
	alignEastButton    = (Qsi::Block*) buttonBox->buttonAdd("AlignEast",      "Align To East",        0, 290, 150, 25, buttonClicked, NULL);
	alignCenterButton  = (Qsi::Block*) buttonBox->buttonAdd("AlignCenter",    "Align Center",         0, 320, 150, 25, buttonClicked, NULL);
	alignNoneButton    = (Qsi::Block*) buttonBox->buttonAdd("CompressButton", "Unalign",              0, 350, 150, 25, buttonClicked, NULL);

	textButton->boxMoveSet(true);
	
	//
	// Input
	//
	Qsi::Box* inputBox = (Qsi::Box*) mainBox->boxAdd("inputBox", 10, 650);

	inputP             = (Qsi::Block*) inputBox->inputAdd("Input1",          "Input1",              0, 0, 200, 50);
	inputOkButton      = (Qsi::Block*) inputBox->buttonAdd("Input1OK",       "OK",                  0, 0,  50, 25);

	inputOkButton->align(Qsi::Alignment::East, inputP, 20);



	//
	// Small table
	//
	Qsi::Block*  numberImageP;
	Qsi::Box*    smallTableBox = (Qsi::Box*) mainBox->boxAdd("smallTableBox", 700, 150);

	tableImageP      = (Qsi::Block*) smallTableBox->imageAdd("Table",  "Database.png", 0, 0, -1, -1);
	numberImageP     = (Qsi::Block*) smallTableBox->imageAdd("Number", "seven.png", 50, 40, 20, 20);

	numberImageP->align(Qsi::Alignment::Center, tableImageP, 0);
	numberImageP->moveRelative(0, -45);
	tableImageP->boxMoveSet(true);
	numberImageP->boxMoveSet(true);


	//
	// Big table
	//
	bigTableImageP = (Qsi::Block*) mainBox->imageAdd("Big Table",  "Database.png", 600, 400, 300, 300, buttonClicked, NULL);



	//
	// Rectangle
	//
	rectangle = (Qsi::Block*) mainBox->rectangleAdd("Rectangle", 400, 400, 300, 200, QColor(0xFF, 0xFF, 0xFF, 0x80), QColor(0, 0, 0, 0xFF), 3, buttonClicked, NULL);


	//
	// InputLine
	//
	inputLine = new Qsi::InputLine(mainBox, "Input Test", "", "OK", 400, 300, 10, 10, inputLineFunc);
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
