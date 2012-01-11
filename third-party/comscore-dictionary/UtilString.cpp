#include "stdafx.h"
#include "UtilString.h"
//For Strings
void util::_ToLower( string& str ) 
{

	int ( *ptrFun )( int ) = tolower; 
	transform( str.begin(), str.end(), str.begin(), ptrFun ); 
	
}

void util::_ToLwr( char* str )
{

	while ( 0 != ( *str++ = (char)tolower( *str ) ) );
	
}
