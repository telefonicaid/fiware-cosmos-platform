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
* In-data for the test program
*/
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
Qsi::QsiBlock*    fontButton;
Qsi::QsiBlock*    colorButton;
Qsi::QsiBlock*    boldButton;
Qsi::QsiBlock*    italicButton;

Qsi::QsiBlock*    alignSouthButton;
Qsi::QsiBlock*    alignNorthButton;
Qsi::QsiBlock*    alignWestButton;
Qsi::QsiBlock*    alignEastButton;
Qsi::QsiBlock*    alignCenterButton;
Qsi::QsiBlock*    alignNoneButton;


Qsi::QsiBox*      qBox[10];
Qsi::QsiBlock*    qBoxTitle[10];


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



void colorChange(Qsi::QsiBlock* qbP, void* col)
{
	char* color = (char*) col;

	if (strcmp(color, "Red") == 0)
		qbP->setColor(0xFF, 0, 0, 0xFF);
	else if (strcmp(color, "Green") == 0)
		qbP->setColor(0, 0xFF, 0, 0xFF);
	else if (strcmp(color, "Blue") == 0)
		qbP->setColor(0, 0, 0xFF, 0xFF);
	else
		qbP->setColor(0, 0, 0, 0xFF);
}



void remove(Qsi::QsiBlock* qbP, void* x)
{
	Qsi::QsiBox* box = qbP->getOwner();

	box->remove(qbP);
}



void alignmentList(Qsi::QsiBlock* qbP, void* x)
{
	Qsi::QsiBox*  box   = (Qsi::QsiBox*) x;

	box->alignShow("From Menu", true);
}



void qsiList(Qsi::QsiBlock* qbP, void* x)
{
	Qsi::QsiBox*  box   = (Qsi::QsiBox*) x;

	box->qsiShow("From Menu", true);
}



void qsiAllList(Qsi::QsiBlock* qbP, void* x)
{
	Qsi::QsiBox*  box   = (Qsi::QsiBox*) x;

	box->qsiRecursiveShow("From Menu", true);
}



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
}



/* ****************************************************************************
*
* buttonClicked - 
*/
void buttonClicked(Qsi::QsiBlock* qbP, void* param)
{
	QPushButton* button;
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
		user2->getOwner()->align(Qsi::Alignment::South, user1->getOwner(), 20);
	else if (qbP == alignNorthButton)
		user2->getOwner()->align(Qsi::Alignment::North, user1->getOwner(), 20);
	else if (qbP == alignWestButton)
		user2->getOwner()->align(Qsi::Alignment::West, user1->getOwner(), 20);
	else if (qbP == alignEastButton)
		user2->getOwner()->align(Qsi::Alignment::East, user1->getOwner(), 20);
	else if (qbP == alignCenterButton)
		user2->getOwner()->align(Qsi::Alignment::Center, user1->getOwner(), 20);
	else if (qbP == alignNoneButton)
		user2->getOwner()->unalign(user1->getOwner());
	else if (qbP == inputOkButton)
		LM_T(LmtInput, ("Input Test: '%s'", inputP->getText()));
	else
		LM_W(("No button found"));
}



void compress(Qsi::QsiBlock* qbP, void* vP)
{
	for (int ix = 0; ix < 10; ix++)
		qBoxTitle[ix]->hideOthers();

	qbP = NULL;
	vP  = NULL;
}



void expand(Qsi::QsiBlock* qbP, void* vP)
{
	for (int ix = 0; ix < 10; ix++)
		qBoxTitle[ix]->showOthers();

	qbP = NULL;
	vP  = NULL;
}



/* ****************************************************************************
*
* qsiSetup - 
*/
static void qsiSetup(QWidget* mainWindow)
{
	QVBoxLayout*  layout = new QVBoxLayout();
	int           x,y,w,h;

	mainWindow->setLayout(layout);

	qsiManager = new Qsi::QsiManager(layout, "./TestImages", "Background.png", 1920, 1080);

	Qsi::QsiBox*   mainBox   = qsiManager->box;
	Qsi::QsiBox*   qMainBox  = (Qsi::QsiBox*) mainBox->boxAdd("qMainBox", 1100, 100);
	Qsi::QsiBox*   userBox1  = (Qsi::QsiBox*) mainBox->boxAdd("userBox1", 500, 200);
	Qsi::QsiBox*   userBox2  = (Qsi::QsiBox*) mainBox->boxAdd("userBox2", 200,  20);


	qsiManager->menuAdd("Alignment List", alignmentList, mainBox);
	qsiManager->menuAdd("Qsi List",       qsiList,       mainBox);
	qsiManager->menuAdd("All Qsis",       qsiAllList,    mainBox);
	qsiManager->menuAdd("Compress All",   compress,      NULL);
	qsiManager->menuAdd("Expand All",     expand,        NULL);

	//
	// Test with 10 aligned boxes
	//
	for (int ix = 0; ix < 10; ix++)
	{
		char            boxName[16];
		char            txt[32];
		Qsi::QsiBlock*  qb1P;
		Qsi::QsiBlock*  qb2P;
		Qsi::QsiBlock*  qb3P;
		Qsi::QsiBlock*  qb4P;

		snprintf(boxName, sizeof(boxName), "QBox %d", ix);
		qBox[ix] = (Qsi::QsiBox*) qMainBox->boxAdd(boxName, 0, ix * 70);

		qBoxTitle[ix] = (Qsi::QsiBlock*) qBox[ix]->textAdd(boxName, boxName, 0, 0);
		qBoxTitle[ix]->setBold(true);
		qsiManager->siConnect(qBoxTitle[ix], textClicked, NULL);
		qBoxTitle[ix]->menuAdd("Alignment List", alignmentList, qBox[ix]);

		snprintf(txt,     sizeof(txt),     "%d KVs", 19 + ix * 17);
		qb1P = (Qsi::QsiBlock*) qBox[ix]->textAdd(txt, txt, 10, 20);
		qb1P->menuAdd("Remove", remove, qb1P); 
		qb1P->menuAdd("Red",   colorChange, (void*) "Red");
		qb1P->menuAdd("Green", colorChange, (void*) "Green");
		qb1P->menuAdd("Blue",  colorChange, (void*) "Blue");
		qb1P->menuAdd("Black", colorChange, (void*) "Black");
		qb1P->align(Qsi::Alignment::South, qBoxTitle[ix], 10);
		
		snprintf(txt,     sizeof(txt),     "%d Mbytes", 37 + ix * 13);
		qb2P = (Qsi::QsiBlock*) qBox[ix]->textAdd(txt, txt, 10, 35);
		qb2P->menuAdd("Remove", remove, qb2P); 
		qb2P->menuAdd("Red",   colorChange, (void*) "Red");
		qb2P->menuAdd("Green", colorChange, (void*) "Green");
		qb2P->menuAdd("Blue",  colorChange, (void*) "Blue");
		qb2P->menuAdd("Black", colorChange, (void*) "Black");
		qb2P->align(Qsi::Alignment::South, qb1P, 5);

		snprintf(txt,     sizeof(txt),     "%d Mbps", 7 + ix * 3);
		qb3P = (Qsi::QsiBlock*) qBox[ix]->textAdd(txt, txt, 10, 50);
		qb3P->menuAdd("Remove", remove, qb3P); 
		qb3P->menuAdd("Red",   colorChange, (void*) "Red");
		qb3P->menuAdd("Green", colorChange, (void*) "Green");
		qb3P->menuAdd("Blue",  colorChange, (void*) "Blue");
		qb3P->menuAdd("Black", colorChange, (void*) "Black");
		qb3P->align(Qsi::Alignment::South, qb2P, 5);

		snprintf(txt,     sizeof(txt),     "%d seconds", 137 + ix * 13);
		qb4P = (Qsi::QsiBlock*) qBox[ix]->textAdd(txt, txt, 10, 65);
		qb4P->menuAdd("Remove", remove, qb4P); 
		qb4P->menuAdd("Red",   colorChange, (void*) "Red");
		qb4P->menuAdd("Green", colorChange, (void*) "Green");
		qb4P->menuAdd("Blue",  colorChange, (void*) "Blue");
		qb4P->menuAdd("Black", colorChange, (void*) "Black");
		qb4P->align(Qsi::Alignment::South, qb3P, 5);

		if (ix > 0)
			qBox[ix]->align(Qsi::Alignment::South, qBox[ix - 1], 10);
	}

	LM_M(("Setting frame for qMainBox"));
	qMainBox->setFrame("QBoxes", 10);
	
	//
	// User 1
	//
	user1   = (Qsi::QsiBlock*) userBox1->textAdd("User1", "Ken Zangelin", 0, 0);
	user1->geometry(&x,&y,&w,&h);
	userBox1->lineAdd("delimiter1", 0, h, w, 0);
	user1->setBold(true);

	for (unsigned int ix = 0; ix < sizeof(props1) / sizeof(props1[0]); ix++)
		userBox1->textAdd(props1[ix], props1[ix],   10, (ix + 1) * 20 + 5);

	userBox1->geometry(&x,&y,&w,&h);
	LM_M(("Geometry for %s: { %d, %d } %d x %d", userBox1->name, x,y,w,h));
	userBox1->lineAdd("delimiter2", 0, h, w, 0);

	user1->menuAdd("Edit User",    userEdit,   user1);
	user1->menuAdd("Remove User",  userRemove, user1);
	user1->menuAdd("Color Change", userColor,  user1);

	qsiManager->siConnect(user1, textClicked, NULL);



	//
	// User 2
	//
	user2 = (Qsi::QsiBlock*) userBox2->textAdd("User2", "Alex Zangelin Calvo", 0, 0);
	user2->geometry(&x,&y,&w,&h);
	userBox2->lineAdd("delimiter3", 0, h, w, 0);
	user2->setBold(true);

	for (unsigned int ix = 0; ix < sizeof(props2) / sizeof(props2[0]); ix++)
		userBox2->textAdd(props2[ix], props2[ix],   10, (ix + 1) * 20 + 5);

	userBox2->geometry(&x,&y,&w,&h);
	LM_M(("Geometry for %s: { %d, %d } %d x %d", userBox2->name, x,y,w,h));
	userBox2->lineAdd("delimiter4", 0, h, w, 0);

	user2->menuAdd("Edit User",    userEdit,   user2);
	user2->menuAdd("Remove User",  userRemove, user2);
	user2->menuAdd("Color Change", userColor,  user2);

	qsiManager->siConnect(user2, textClicked, NULL);



	//
	// Buttons
	//
	Qsi::QsiBox* buttonBox = (Qsi::QsiBox*) mainBox->boxAdd("buttonBox", 10, 10);

	textButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("TextButton",     "Change Text",          0,  10, 150, 25, buttonClicked, user1);
	moveButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("MoveButton",     "Move User Group",      0,  40, 150, 25, buttonClicked, NULL);
	fontButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("FontButton",     "Font",                 0,  70, 150, 25, buttonClicked, NULL);
	colorButton        = (Qsi::QsiBlock*) buttonBox->buttonAdd("ColorButton",    "Color",                0, 100, 150, 25, buttonClicked, NULL);
	boldButton         = (Qsi::QsiBlock*) buttonBox->buttonAdd("BoldButton",     "Bold",                 0, 130, 150, 25, buttonClicked, NULL);
	italicButton       = (Qsi::QsiBlock*) buttonBox->buttonAdd("ItalicButton",   "Italic",               0, 160, 150, 25, buttonClicked, NULL);

	alignSouthButton   = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignSouth",     "Align South",          0, 200, 150, 25, buttonClicked, NULL);
	alignNorthButton   = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignNorth",     "Align North",          0, 230, 150, 25, buttonClicked, NULL);
	alignWestButton    = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignWest",      "Align To West",        0, 260, 150, 25, buttonClicked, NULL);
	alignEastButton    = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignEast",      "Align To East",        0, 290, 150, 25, buttonClicked, NULL);
	alignCenterButton  = (Qsi::QsiBlock*) buttonBox->buttonAdd("AlignCenter",    "Align Center",         0, 320, 150, 25, buttonClicked, NULL);
	alignNoneButton    = (Qsi::QsiBlock*) buttonBox->buttonAdd("CompressButton", "Unalign",              0, 350, 150, 25, buttonClicked, NULL);


	
	//
	// Input
	//
	Qsi::QsiBox* inputBox = (Qsi::QsiBox*) mainBox->boxAdd("inputBox", 10, 650);

	inputP             = (Qsi::QsiBlock*) inputBox->inputAdd("Input1",          "Input1",              0, 0, 200, 50);
	inputOkButton      = (Qsi::QsiBlock*) inputBox->buttonAdd("Input1OK",       "OK",                  0, 0,  50, 25);

	inputOkButton->align(Qsi::Alignment::East, inputP, 20);



	//
	// Small table
	//
	Qsi::QsiBlock*  numberImageP;
	Qsi::QsiBox*    smallTableBox = (Qsi::QsiBox*) mainBox->boxAdd("smallTableBox", 700, 150);

	tableImageP      = (Qsi::QsiBlock*) smallTableBox->imageAdd("Table",  "Database.png", 0, 0, -1, -1);
	numberImageP     = (Qsi::QsiBlock*) smallTableBox->imageAdd("Number", "seven.png", 50, 40, 20, 20);

	numberImageP->align(Qsi::Alignment::Center, tableImageP, 0);
	numberImageP->moveRelative(0, -45);



	//
	// Big table
	//
	bigTableImageP = (Qsi::QsiBlock*) mainBox->imageAdd("Big Table",  "Database.png", 600, 400, 300, 300, buttonClicked, NULL);
	bigTableImageP->setBoxMove(false);
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

