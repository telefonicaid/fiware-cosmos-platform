#ifndef PA_IS_SET_H
#define PA_IS_SET_H

/* ****************************************************************************
*
* FILE                  paIsSet.h
*
* AUTHOR                Ken Zangelin
*
*/



/* ****************************************************************************
*
* paIsSet - is an argument existing in the parse list
*/
extern bool paIsSet(int argC, char* argV[], const char* option);



/* ****************************************************************************
*
* paIsSetSoGet - return value of option 'option'
*/
extern const char* paIsSetSoGet(int argC, char* argV[], const char* option);

#endif
