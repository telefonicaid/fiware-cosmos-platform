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
#include "QsiDialog.h"          // Qsi::Dialog
#include "QsiInputLine.h"       // Qsi::InputLine
#include "QsiInputDialog.h"     // Qsi::InputDialog



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

static Qsi::Block*       scrollUpButton;
static Qsi::Block*       scrollDownButton;
static Qsi::Box*         scrollBox;
static Qsi::Box*         verticalBox;

static Qsi::Block*       rectangle;
static Qsi::InputLine*   inputLine;
static Qsi::Block*       combo;



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
* qlist variables
*/
#define QLIST_EXP_LISTS          10
#define QLIST_EXP_LIST_MEMBERS   6

static Qsi::Block*       qlistRec;
static Qsi::Block*       qlistTitle;
static Qsi::Box*         qlistBox;
static Qsi::ExpandList*  qlistExpList[QLIST_EXP_LISTS];



/* ****************************************************************************
*
* qlistCallback - 
*/
static void qlistCallback(Qsi::Block* qbP, void* vP, const char* nada)
{
	char* txt = (char*) vP;

	LM_M(("QList item '%s' pressed: '%s'", qbP->name, txt));

	if (strcmp(txt, "Remove") == 0)
		remove(qbP, vP);

	nada = NULL;
}



/* ****************************************************************************
*
* qlistCreate - 
*/
static void qlistCreate(void)
{
	qlistRec    = (Qsi::Block*) mainBox->rectangleAdd("QListRec", 1400, 0, 400, 100, QColor(0x80, 0x90, 0x80, 0xFF), QColor(0, 0, 0, 0xFF), 3, qlistCallback, NULL);
	qlistTitle  = (Qsi::Block*) mainBox->textAdd("Queue X", "Queue X",      0, 0);
	
	qlistTitle->align(Qsi::Alignment::Center, qlistRec, 0);

	qlistRec->setZValue(0.80);
	qlistTitle->setZValue(0.81);
	
	qlistBox = (Qsi::Box*) mainBox->boxAdd("QList Scroll Box", 1400, 100);
	qlistBox->setVertical(true);
	mainBox->scrollAreaSet(qlistBox, 1400, -2000, 400, 4000, 20, true);

	for (int eix = 0; eix < QLIST_EXP_LISTS; eix++)
	{
		char        name[32];
		const char* subMenuItem[] =
		{
			"Remove",
			"Move",
			"Red",
			"Green",
			"Gray",
			NULL
		};

		sprintf(name, "Expand List %02d", eix);
		qlistExpList[eix] = new Qsi::ExpandList(qsiManager, qlistBox, name, 0, 0, 0, 0, qlistCallback);
		for (int mix = 0; mix < QLIST_EXP_LIST_MEMBERS; mix++)
		{
			sprintf(name, "Item %02d-%02d", eix, mix);
			qlistExpList[eix]->addMember(name, qlistCallback, name, subMenuItem);
		}
	}
}



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
* alignmentList - 
*/
static void alignmentList(Qsi::Block* qbP, void* x, const char* nada)
{
	Qsi::Box*  box   = (Qsi::Box*) x;

	box->alignShow("From Menu", true);

	nada = NULL;
}



/* ****************************************************************************
*
* qsiList - 
*/
static void qsiList(Qsi::Block* qbP, void* x, const char* nada)
{
	Qsi::Box*  box   = (Qsi::Box*) x;

	box->qsiShow("From Menu", true);

	nada = NULL;
}



/* ****************************************************************************
*
* qsiAllList - 
*/
static void qsiAllList(Qsi::Block* qbP, void* x, const char* nada)
{
	Qsi::Box*  box   = (Qsi::Box*) x;

	box->qsiRecursiveShow("From Menu", true);

	nada = NULL;
}



/* ****************************************************************************
*
* buttonClicked - 
*/
static void buttonClicked(Qsi::Block* qbP, void* param, const char* nada)
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
	else if (qbP == scrollUpButton)
		scrollBox->scroll(10);
	else if (qbP == scrollDownButton)
		scrollBox->scroll(-10);
	else
		LM_W(("No button found"));

	nada = NULL;
}



/* ****************************************************************************
*
* compress - 
*/
static void compress(Qsi::Block* qbP, void* vP, const char* nada)
{
	userBox1->compress();
	userBox2->compress();
	elist->compress();

	qbP  = NULL;
	vP   = NULL;
	nada = NULL;
}



/* ****************************************************************************
*
* expand - 
*/
static void expand(Qsi::Block* qbP, void* vP, const char* nada)
{
	userBox1->expand();
	userBox2->expand();
	elist->expand();

	qbP  = NULL;
	vP   = NULL;
	nada = NULL;
}



/* ****************************************************************************
*
* dialog - 
*/
static void dialog(Qsi::Block* qbP, void* vP, const char* nada)
{
	bool         modal = false;
	char*        m     = (char*) vP;
	const char*  title;

	if (strcmp(m, "modal") == 0)
		modal = true;

	title = (modal == false)? "Testing Non-MODAL Dialog" : "Testing MODAL Dialog"; 

	new Qsi::Dialog(qsiManager, title, modal);
	vP   = NULL;
	nada = NULL;
}



char** inputDialogOutput;
/* ****************************************************************************
*
* inputDialogGo - 
*/
void inputDialogGo(char* texts[], char* results[])
{
	LM_M(("Got a user:"));
	for (int ix = 0; texts[ix] != NULL; ix++)
	{
		LM_M(("%-30s: %s", texts[ix], results[ix]));
		free(texts[ix]);
		free(results[ix]);
	}

	free(inputDialogOutput);
}



/* ****************************************************************************
*
* inputDialog - 
*/
static void inputDialog(Qsi::Block* qbP, void* vP, const char* nada)
{
	bool   modal = false;
	char*  m     = (char*) vP;

	const char* texts[] = 
	{
		"User Name",
		"First Name",
		"Last Name",
		"Street Address",
		"Town Address",
		"Phone",
		"Cell Phone",
		NULL
	};
	
	inputDialogOutput = (char**) calloc(sizeof(texts) / sizeof(texts[0]), sizeof(char*));

	if (strcmp(m, "modal") == 0)
		modal = true;

	const char* comboContent[] = 
	{
		(char*) "Normal User",
		(char*) "Owner",
		(char*) "Expert User",
		(char*) "No User"
	};

	new Qsi::InputDialog
	(
		qsiManager,
		"Create New User",
		(char**) texts,
		inputDialogOutput,
		"Create User",
		comboContent,
		modal,
		inputDialogGo
	);

	qbP  = NULL;
	vP   = NULL;
	nada = NULL;
}



/* ****************************************************************************
*
* elistCallback - 
*/
static void elistCallback(Qsi::Block* qbP, void* vP, const char* nada)
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

	nada = NULL;
}



/* ****************************************************************************
*
* userCallback - 
*/
static void userCallback(Qsi::Block* qbP, void* vP, const char* nada)
{
	char* txt = (char*) vP;

	LM_M(("ExpandList item '%s' pressed: '%s'", qbP->name, txt));

	if (strcmp(txt, "Edit Field") == 0)
		fieldEdit(qbP, vP);
	else if (strcmp(txt, "Remove") == 0)
		remove(qbP, vP);
	else if (strcmp(txt, "Color Change") == 0)
		userColor(qbP, vP);

	nada = NULL;
}



/* ****************************************************************************
*
* inputLineFunc - 
*/
static void inputLineFunc(char** nameV, char** inputV)
{
	LM_M(("Got input from inputLine:"));
	for (int ix = 0; nameV[ix] != NULL; ix++)
		LM_M(("%s: %s", nameV[ix], inputV[ix]));
}



/* ****************************************************************************
*
* comboCallback - 
*/
static void comboCallback(Qsi::Block* qbP, void* vP, const char* nada)
{
	LM_M(("IN"));

	qbP  = NULL;
	vP   = NULL;
	nada = NULL;
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

	qsiManager->menuAdd("Alignment List",          alignmentList, mainBox);
	qsiManager->menuAdd("Qsi List",                qsiList,       mainBox);
	qsiManager->menuAdd("All Qsis",                qsiAllList,    mainBox);
	qsiManager->menuAdd("Compress All",            compress,      NULL);
	qsiManager->menuAdd("Expand All",              expand,        NULL);
	qsiManager->menuAdd("Modal Dialog",            dialog,        (void*) "modal");
	qsiManager->menuAdd("Non-modal Dialog",        dialog,        (void*) "non-modal");
	qsiManager->menuAdd("Modal Input Dialog",      inputDialog,   (void*) "modal");
	qsiManager->menuAdd("Non-modal Input Dialog",  inputDialog,   (void*) "non-modal");

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

	scrollUpButton     =  (Qsi::Block*) buttonBox->buttonAdd("ScrollUp",      "Scroll Up",            0, 390, 150, 25, buttonClicked, NULL);
	scrollDownButton   =  (Qsi::Block*) buttonBox->buttonAdd("ScrollDown",    "Scroll Down",          0, 420, 150, 25, buttonClicked, NULL);

	textButton->boxMoveSet(true);
	
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


	//
	// Combo
	//
	const char* option[] =
	{
		"Red",
		"Green",
		"Blue",
		"Yellow",
		"Magenta",
		"Cyan",
		"Black",
		NULL
	};

	combo = (Qsi::Block*) mainBox->comboAdd("Combo", option, 400, 700, -1, -1, comboCallback, NULL);


	//
	// ScrollBox
	//
	scrollBox = (Qsi::Box*) mainBox->boxAdd("ScrollBox", 1000, 30);

	for (int ix = 0; ix < 50; ix++)
	{
		char txt[32];

		sprintf(txt, "Line %d", ix);
		scrollBox->textAdd(txt, txt, 0, ix * 20);
	}

	// scrollBox->setScrollable(true);   NOT necessary
	LM_M(("Creating scroll area for scrollBox: { %d, %d } %dx%d", 1000, 30, 200, 500));
	mainBox->scrollAreaSet(scrollBox, 1000, 100, 200, 300, 10, true);


	//
	// VerticalBox
	//
	Qsi::Block* b;

	verticalBox = (Qsi::Box*) mainBox->boxAdd("VerticalBox", 1300, 30);
	verticalBox->setVertical(true);

	b = (Qsi::Block*) verticalBox->textAdd("Line1", "Line1", 0, 0);
	b->boxMoveSet(true);

	verticalBox->textAdd("Line2", "Line2", 0, 0);
	verticalBox->textAdd("Line3", "Line3", 0, 0);
	verticalBox->textAdd("Line4", "Line4", 0, 0);
	verticalBox->textAdd("Line5", "Line5", 0, 0);

	qlistCreate();
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
