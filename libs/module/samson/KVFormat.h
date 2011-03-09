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

#pragma mark ************************************************
#pragma mark KVFormat key-value format definition
#pragma mark ************************************************

namespace ss {

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
		
		static KVFormat format( std::string key_format , std::string value_format )
		{
			KVFormat tmp;
			tmp.init( key_format , value_format );
			return tmp;
		}
		
	};
}

#endif
