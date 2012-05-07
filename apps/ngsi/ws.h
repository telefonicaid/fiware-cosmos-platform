#ifndef WS_H
#define WS_H


/* ****************************************************************************
*
* isWs - 
*/
extern bool isWs(char c);



/* ****************************************************************************
*
* wsClean - remove starting and trailing whitespace
*/
extern char* wsClean(char* in);



/* ****************************************************************************
*
* onlyWS - 
*/
extern bool onlyWS(char* s);

#endif
