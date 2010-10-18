#ifndef AU_TOKENIZER_H
#define AU_TOKENIZER_H

/* ****************************************************************************
*
* FILE                     AUTockenizer.h
*
* DESCRIPTION			   Tockenizer to parse files describing modules
*
*/



namespace ss {
	
	/**
	 General tokenizer element
	 */
	
	class AUTockenizer
	{
		
	public:
		std::vector<std::string> items;			// Parsed elements from the text

		
		AUTockenizer( std::string txt )
		{
			// First tokenize everything
			std::vector<std::string> previous_items = tockenize(txt);
			
			// Remove spaces and returns.
			// Spaces between " " are not removed
			items = removeSpacesAndReturns( removeComments(previous_items) );
		}
		
		static std::vector<std::string> removeSpacesAndReturns( std::vector<std::string> items )
		{
			std::vector<std::string> new_items;
			
			bool literal = false;
			
			std::vector<std::string>::iterator iter;
			
			for ( iter = items.begin() ; iter < items.end() ; iter++)
			{
				if( literal )
				{
					if ( *iter == "\"" )
						literal = false;
					new_items.push_back( *iter );
					
				}
				else
				{
					if ( *iter == "\"" )
					{
						literal = true;
						new_items.push_back( *iter );
					}
					else
					{
						// Outside literals, we do not have "returns" "spaces" "tabs" "\r"
						if( !isOneOf(*iter, " \t\r\n") )
							new_items.push_back( *iter );
						
					}
					
				}
			}
			
			return new_items;
			
		}
		
		/** 
		 Function to remove comments from tockens
		 Bascially removes every tocken between # and end_of_line
		 */
		
		
		static std::vector<std::string> removeComments( std::vector<std::string> items )
		{
			std::vector<std::string> new_items;
			
			bool removing = false;
			
			std::vector<std::string>::iterator iter;
			
			for ( iter = items.begin() ; iter < items.end() ; iter++)
			{
				if( removing )
				{
					if ( *iter == "\n" )
					{
						removing = false;
						new_items.push_back( *iter );	//Keep the return
					}
				}
				else
				{
					if ( *iter == "#" )
						removing = true;
					else
						new_items.push_back( *iter );
				}
			}
			
			return new_items;
			
		}
		
		/**
		 
		 Functions to tockenize the input string
		 
		 */
		
		static std::vector<std::string> tockenize( std::string txt)
		{
			std::string tockens = " #\r\t\r\n{};\"";//All possible delimiters
			
			std::vector<std::string> items;
			
			// Simple parser
			size_t pos = 0;
			for (size_t i = 0 ; i < txt.size() ; i++)
			{
				if ( isOneOf( txt[i] , tockens ) )
				{
					if ( i > pos )
						items.push_back(txt.substr(pos, i - pos ));
					
					//Emit the literal with one letter if that is the case
					ostringstream o;
					o << txt[i];
					items.push_back( o.str() );
					
					pos = i+1;
				}
			}
			
			
			return items;
		}
		
		/**
		 Auxiliar functions
		 */
		
		static bool isOneOf( char c , std::string s )
		{
			for (size_t i = 0 ; i < s.size() ; i++)
				if( s[i] == c )
					return true;
			
			return false;
			
		}
		
		static bool isOneOf( std::string c_string , std::string s )
		{
			if (c_string.size() > 1)
				return false;
			
			char c = c_string[0];
			
			
			for (size_t i = 0 ; i < s.size() ; i++)
				if( s[i] == c )
					return true;
			
			return false;
			
		}
		
		/**
		 Auxiuliar function to get the "i-th" input item
		 */
		
		std::string itemAtPos( unsigned int pos )
		{
			assert( pos >= 0 );
			assert( pos < items.size() );
			
			return items[pos];
		}
		
		
		bool isSpecial( int pos )
		{
			std::string tmp = itemAtPos(pos);
			return isOneOf(tmp, "{};");
		}
		
		bool isOpenSet( int pos )
		{
			return( itemAtPos(pos) == "{" );
		}
		
		bool isCloseSet( int pos )
		{
			return( itemAtPos(pos) == "}" );
		}
		
		bool isSemiColom( int pos )
		{
			return( itemAtPos(pos) == ";" );
		}
		
		bool isOpenCloseLiteral( int pos )
		{
			return( itemAtPos(pos) == "\"" );
		}
		
		int searchSetFinishStartingAt( int pos )
		{
			int number_intern_sets = 1;
			
			assert( isOpenSet(pos) );
			pos++;
			while( number_intern_sets > 0 )
			{
				if( isCloseSet(pos) )
					number_intern_sets--;
				else if( isOpenSet(pos) )
					number_intern_sets++;
				
				pos++;
			}
			
			return (pos-1);
		}
		
		int searchCloseLiteral( int pos )
		{
			pos++;
			while( !isOpenCloseLiteral(pos) )
				pos++;
			
			return pos;
		}
		
		std::string getLiteralInternal( int pos , int pos2)
		{
			ostringstream o;
			for (int i = (pos+1) ; i < pos2 ; i++)
			{
				o << itemAtPos(i);
			}
			return o.str();
		}
		
		std::string getLiteral( int* pos )
		{
			if( isOpenCloseLiteral(*pos) )
			{
				int pos_start = *pos;
				int pos_finish = searchCloseLiteral(*pos);
				*pos = pos_finish + 1;
				return getLiteralInternal(pos_start, pos_finish);
			}
			else
			{
				int pos_item = *pos;
				*pos = *pos + 1;
				return itemAtPos(pos_item);
			}
		}
		
		
		void getScopeLimits( int* pos , int*begin, int*end )
		{		
			assert( isOpenSet(*pos) );
			*begin = *pos + 1;
			
			int tmp	= searchSetFinishStartingAt( *pos ) ;
			
			*end = tmp - 1;
			*pos = tmp + 1;
			
		}
	};
	
}

#endif
