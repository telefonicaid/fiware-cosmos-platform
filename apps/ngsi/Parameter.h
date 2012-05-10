#ifndef PARAMETER_H
#define PARAMETER_H

/* ****************************************************************************
*
* FILE                     Parameter.h - 
*
*
*
*/



/* ****************************************************************************
*
* Parameter - 
*/
typedef struct Parameter
{
    char* name;
    char* value;
} Parameter;



/* ****************************************************************************
*
* parametersParse - 
*/
extern void parametersParse(char* params);



/* ****************************************************************************
*
* parametersPresent - 
*/
extern void parametersPresent(void);

#endif
