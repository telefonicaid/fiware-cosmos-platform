#ifndef AU_TOKENIZER_H
#define AU_TOKENIZER_H


/* ****************************************************************************
*
* FILE                     AUTockenizer.h
*
* DESCRIPTION			   Tockenizer to parse files describing modules
*
*/

#include <vector>		// std::vector
#include <string>		// std::string

namespace ss {
	
	/**
	 General tokenizer element
	 */
	
	class AUTockenizer
	{
		
	public:
		std::vector<std::string> items;			// Parsed elements from the text

		
		AUTockenizer( std::string txt );
		
		static std::vector<std::string> removeSpacesAndReturns( std::vector<std::string> items );		
		/** 
		 Function to remove comments from tockens
		 Bascially removes every tocken between # and end_of_line
		 */
		
		static std::vector<std::string> removeComments( std::vector<std::string> items );
		
		static std::vector<std::string> tockenize( std::string txt);		
		
		static bool isOneOf( char c , std::string s );
		
		static bool isOneOf( std::string c_string , std::string s );
		
		std::string itemAtPos( unsigned int pos );
		
		bool isSpecial( int pos );
		
		bool isOpenSet( int pos );
		
		bool isCloseSet( int pos );
		
		bool isSemiColom( int pos );
		
		bool isOpenCloseLiteral( int pos );
		
		int searchSetFinishStartingAt( int pos );

		int searchCloseLiteral( int pos );
		
		std::string getLiteralInternal( int pos , int pos2);
		
		std::string getLiteral( int* pos );
		
		void getScopeLimits( int* pos , int*begin, int*end );
		
	};
	
}

#endif
