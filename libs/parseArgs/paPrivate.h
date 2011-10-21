#ifndef PA_PRIVATE_H
#define PA_PRIVATE_H

/* ****************************************************************************
*
* FILE                  paPrivate.h
*
* AUTHOR                Ken Zangelin
*
*/
#include "baStd.h"         /* BaBoolean                                      */
#include "parseArgs/parseArgs.h"     /* PaArgument                                     */



/* ****************************************************************************
*
* PA_IS_XXX
*/
#define PA_IS_OPTION(argP)      ((argP->what & PawOption)    == PawOption)
#define PA_IS_PARAMETER(argP)   ((argP->what & PawParameter) == PawParameter)
#define PA_IS_VARIABLE(argP)    ((argP->what & PawVariable)  == PawVariable)



/* ****************************************************************************
*
* PaWhat - 
*/
typedef enum PaWhat
{
   PawOption    = (1 << 0),
   PawParameter = (1 << 1),
   PawVariable  = (1 << 2)
} PaWhat;



/* ****************************************************************************
*
* PaTypeUnion - 
*/
typedef union PaTypeUnion
{
   char             c;
   unsigned char   uc;
   short            s;
   unsigned short  us;
   int              i;
   unsigned int    ui;
   char*           string;
   unsigned char*  ustring;
   bool            boolean;
} PaTypeUnion;



/* ****************************************************************************
*
* paBuiltin - 
*/
extern PaArgument paBuiltin[];



/* ****************************************************************************
*
* stoi - convert string to integer
*
* PREFIXES
*   B:    Boolean
*   0:    Octal
*   H:    Hexadecimal
*   H':   Hexadecimal
*   0x:   Hexadecimal
*/
extern int stoi(char* string);

#endif