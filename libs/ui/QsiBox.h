#ifndef QSI_BOX_H
#define QSI_BOX_H

/* ****************************************************************************
*
* FILE                     QsiBox.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 13 2011
*
*/
#include "QsiBase.h"            // QsiBase
#include "QsiFunction.h"        // QsiFunction
#include "QsiAlignment.h"       // QsiAlignment
#include "QsiFrame.h"           // Qsi::Frame



class QGraphicsItem;



namespace Qsi
{



class QsiBlock;
class QsiManager;
/* ****************************************************************************
*
* QsiBox - 
*/
class QsiBox : public QsiBase
{
private:
	QsiManager*  manager;
	QsiBase**    qsiVec;
	int          qsiVecSize;
	Alignment**  alignVec;
	int          alignVecSize;

	int          xAbs(void);
	int          yAbs(void);

public:
	QsiBox(QsiManager* manager, QsiBox* owner, const char* name, int x, int y);

	QsiManager* managerGet(void) { return manager; };

	void        moveRelative(int x, int y);                                                  // move all qsis in qsiVec
	void        moveAbsolute(int x, int y);                                                  // move all qsis in qsiVec
	int         geometry(int* xP, int* yP, int* widthP, int* heightP);                       // return geometry of this Box
	void        hide(void);                                                                  // make all aligned qsis move
	void        show(void);                                                                  // make all aligned qsis move

	void        initialMove(Qsi::QsiBase* qbP);                                              // move child to absolute position + relative box position
	void        absPos(int* xP, int* yP);                                                    // get absolute position - recursively
	void        add(QsiBase* qsi);                                                           // add a qsi (Box or Block)
	void        remove(QsiBase* qsi);                                                        // remove a qsi (Box or Block)

	void        align(Alignment::Type type, QsiBase* master, int margin);                    // align this Box to another Box
	void        align(QsiBase* master, Alignment::Type type, QsiBase* slave, int margin);    // align qsis
	void        unalign(int ix);                                                             // remove an identified alignment
	void        unalign(QsiBase* master);                                                    // unalign this Box from another Box
	void        unalign(QsiBase* master, QsiBase* slave);                                    // remove alignment
	void        realign(QsiBase* master, Alignment::Type type, QsiBase* slave, int margin);  // absolute move part of alignment
	void        realign(void);                                                               // after removol, realing all upwards
	void        alignFix(QsiBase* qbP);                                                      // Mend alignments at removal of an item
	Alignment*  alignLookup(QsiBase* master, QsiBase* slave);                                // wont have mor than alignment between two qsis
	void        alignShow(const char* why, bool force = false);                              // Show list of alignment vector
	
	void        sizeChange(QsiBase* qsi);                                                    // callback - a child has changed its size

	QsiBlock*   lookup(QGraphicsItem* gItemP);

	QsiBase*    boxAdd(const char*    name,                   int x, int y);
	QsiBase*    textAdd(const char*   name, const char* txt,  int x, int y);
	QsiBase*    lineAdd(const char*   name,                   int x, int y, int x2, int y2);
	QsiBase*    buttonAdd(const char* name, const char* txt,  int x, int y, int width = -1, int height = -1, QsiFunction func = NULL, void* param = NULL);
	QsiBase*    inputAdd(const char*  name, const char* txt,  int x, int y, int width = -1, int height = -1);
	QsiBase*    imageAdd(const char*  name, const char* path, int x, int y, int width = -1, int height = -1, QsiFunction func = NULL, void* param = NULL);
	void        qsiShow(const char* why, bool force = false);
	void        qsiRecursiveShow(const char* why, bool force = false);

	bool        isVisible(void) { return true; }

	void        setFrame(const char* fname, int padding);
	Frame*      frame;
};

}

#endif
