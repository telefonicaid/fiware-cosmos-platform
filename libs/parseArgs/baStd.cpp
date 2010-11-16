#include <stdio.h>              /* sscanf                                    */
#include <stdlib.h>             /* atoi                                      */
#include <string.h>             /* strtoul, strncmp                          */

#include "baStd.h"              /* Own interface                             */



/* ****************************************************************************
*
* baStoi - string to integer
*/
int baStoi(char* string)
{
	if (strncmp(string, "0x", 2) == 0)
		return strtoul(&string[2], NULL, 16);
	else if (strncmp(string, "H'", 2) == 0)
		return strtoul(&string[2], NULL, 16);
	else if (strncmp(string, "H", 1) == 0)
		return strtoul(&string[1], NULL, 16);
	else if (strncmp(string, "0", 1) == 0)
		return strtoul(&string[1], NULL, 8);
	else if (strncmp(string, "B", 1) == 0)
		return strtoul(&string[1], NULL, 2);

	return atoi(string);
}



/* ****************************************************************************
*
* baStof - string float to binary float
*/
float baStof(char* string)
{
	float f;

	sscanf(string, "%f", &f);

	return f;
}



/* ****************************************************************************
*
* baStod - string double to binary double
*/
double baStod(char* string)
{
	double f;

	sscanf(string, "%lf", &f);

	return f;
}



/* ****************************************************************************
*
* baWs - is the character 'c' a whitespace (space, tab or '\n')
*/
bool baWs(char c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\n':
		return true;
		break;
	default:
		return false;
		break;
	}
}



/* ****************************************************************************
*
* baWsNoOf - number of whitespaces in the string 'string'
*/
int baWsNoOf(char* string)
{
	int no = 0;

	while (*string != 0)
	{
		if (baWs(*string) == true)
			++no;
		++string;
	}
	return no;
}



/* ****************************************************************************
*
* baWsStrip - 
*/
char* baWsStrip(char* s)
{
	char* str;
	char* tmP;
	char* toFree;

	if ((s == NULL) || (s[0] == 0))
		return s;

	str = strdup(s);
	if (str == NULL)
	{
		s[0] = 0;
		return s;
	}

	toFree = str;
	while ((*str == ' ') || (*str == '\t'))
		++str;

	tmP = &str[strlen(str) - 1];

	while ((tmP > str) && ((*tmP == ' ') || (*tmP == '\t')))
		--tmP;
	++tmP;
	*tmP = 0;

	if (str[0] != 0)
		strcpy(s, str);
	else
		s[0] = 0;

	free(toFree);

	return s;
}



/* ****************************************************************************
*
* baWsOnly - 
*/
bool baWsOnly(char* str)
{
	while (*str != 0)
	{
		if (baWs(*str) == false)
			return false;
		++str;
	}

	return true;
}



/* ****************************************************************************
*
* baCharCount - 
*/
int baCharCount(char* line, char c)
{
	int noOf = 0;
	while (*line != 0)
	{
		if (*line == c)
			++noOf;
		++line;
	}

	return noOf;
}
