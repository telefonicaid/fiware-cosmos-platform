#ifndef SAMSON_KV_FORMAT_H
#define SAMSON_KV_FORMAT_H

/*
*  KVFormat.h - define key-value format
*  mr_platform
*
*  Created by ANDREU URRUELA PLANAS on 4/12/10.
*  Copyright 2010 TID. All rights reserved.
*
*/
#include <string>
#include <vector>
#include <sstream>

namespace samson {

	class KVFormat
	{
		
	public:
		std::string keyFormat;
		std::string valueFormat;
		
		KVFormat()
		{
		}

		KVFormat( std::string _keyFormat, std::string _valueFormat )
		{
			keyFormat = _keyFormat;
			valueFormat = _valueFormat;
		}
		
		
		void init( std::string _keyFormat, std::string _valueFormat )
		{
			keyFormat = _keyFormat;
			valueFormat = _valueFormat;
		}
				
		void setFromString( std::string key_value )
		{
            if ( key_value == "txt" )
            {
                init( "txt", "txt" );
                return;
            }
            
            
			size_t pos = key_value.find_last_of('-');
			if( pos == std::string::npos )
			{
				init("unknown","unknown");
				return;
			}
			
			std::string key  = key_value.substr( 0 , pos);
			std::string value  = key_value.substr( pos+1 , key_value.length() - pos - 1 );
			
			init( key , value );
		}
		
		static bool compare_format( std::string &format_1 , std::string &format_2 )
		{
		  if( format_1 == "*" )
		    return true;
		  if( format_2 == "*" )
		    return true;
		  return ( format_1 == format_2 );
		}

		bool isEqual(  KVFormat otherFormat )
		{
		  if( !compare_format( keyFormat , otherFormat.keyFormat  ) )
		      return false;
		  if( !compare_format( valueFormat , otherFormat.valueFormat  ) )
		      return false;
			
		  return true;
		}
		
		bool isEqualKey( KVFormat otherFormat )
		{
                  if( !compare_format( keyFormat , otherFormat.keyFormat  ) )
                      return false;
	          return true;
		}
		
		std::string str()
		{
			return keyFormat + "-" + valueFormat;
		}		

		static KVFormat format( std::string string_format )
		{
            KVFormat tmp;
            tmp.setFromString( string_format );
            
            return tmp;
            
            
		}
		
		static KVFormat format( std::string key_format , std::string value_format )
		{
			KVFormat tmp;
			tmp.init( key_format , value_format );
			return tmp;
		}
	
        // get xml information
        void getInfo( std::ostringstream& output )
        {
            output << "<format>\n";
            output << "<key_format>" << keyFormat << "</key_format>\n";
            output << "<value_format>" << valueFormat << "</value_format>\n";
            output << "</format>\n";
        }
        
        bool isTxt()
        {
            return isEqual( KVFormat("txt","txt") );
        }

        
        bool isGenericKVFormat()
        {
            if( keyFormat != "*" )
                return false;
            if( valueFormat != "*" )
                return false;
            return true;
        }
        
        static KVFormat generic()
        {
            return KVFormat("*","*");
        }

        static KVFormat nonCommon()
        {
            return KVFormat("NonCommon","NonCommon");
        }
        
        bool isNonCommonKVFormat()
        {
            return isEqual( KVFormat("NonCommon","NonCommon") );
        }
        
	};
    
    
}

#endif
