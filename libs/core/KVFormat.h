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

		void init( 	std::string _keyFormat, std::string _valueFormat )
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
		
		
		bool isEqual(  KVFormat otherFormat )
		{
			if( keyFormat.compare( otherFormat.keyFormat ) != 0)
				return false;
			if(	valueFormat.compare( otherFormat.valueFormat ) != 0)
				return false;
			
			return true;
		}
		
		bool isEqualKey(  KVFormat otherFormat )
		{
			if( keyFormat.compare( otherFormat.keyFormat ) != 0)
				return false;
			return true;
		}
		
		std::string str()
		{
			return keyFormat + "-" + valueFormat;
		}
		
		
		/**
		 Static function to create a vector of formats
		 */

		static std::vector<KVFormat> vectorOfFormats( KVFormat format1 , KVFormat format2 , KVFormat format3, KVFormat format4 )
		{
			std::vector<KVFormat> formats;
			
			formats.push_back( format1 );
			formats.push_back( format2 );
			formats.push_back( format3 );
			formats.push_back( format4 );
			
			return formats;
		}
		
		
		static std::vector<KVFormat> vectorOfFormats( KVFormat format1 , KVFormat format2 , KVFormat format3 )
		{
			std::vector<KVFormat> formats;
			
			formats.push_back( format1 );
			formats.push_back( format2 );
			formats.push_back( format3 );
			
			return formats;
		}

		static std::vector<KVFormat> vectorOfFormats( KVFormat format1 , KVFormat format2 )
		{
			std::vector<KVFormat> formats;
			
			formats.push_back( format1 );
			formats.push_back( format2 );
			
			return formats;
		}

		static std::vector<KVFormat> vectorOfFormats( KVFormat format1 )
		{
			std::vector<KVFormat> formats;
			
			formats.push_back( format1 );
			
			return formats;
		}
		
		
		static std::vector<KVFormat> vectorOfFormats( KVFormat format , int n_times )
		{
			std::vector<KVFormat> formats;
			
			for (int i = 0 ; i < n_times ; i++)
			formats.push_back( format );
			
			return formats;
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
