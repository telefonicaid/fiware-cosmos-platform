#ifndef SAMSON_DATA_H
#define SAMSON_DATA_H

#include <math.h>						/*                                          */
#include <stdlib.h>						/* malloc                                   */
#include <string.h>						/* memcpy                                   */
#include <string>						/* std::string                              */

#include <samson/KV.h>                  /* KV                                       */
#include <samson/KVVector.h>            /* KVVector                                 */
#include <samson/DataInstance.h>        /* DataInstance                             */



namespace ss {
	
	// Static functions necessary for a Data
	typedef DataInstance* (*DataCreationFunction)();						// Creation function

	// Function used to "sort" when data type is used as key
	typedef int(* DataCompareFunction)(KV *kv1 , KV*kv2);		

	// Function used to get the size of data from the stream of bytes
	typedef int(* DataSizeFunction)(char *data);
	
	// Templatd function to create an instance of DataInstance
	template <class T>
	DataInstance* getDataIntace()
	{
		return new T();
	}
	
	/** 
	 Definition of a new type of data
	 */
	
	class Data
	{		
		
	public:

		std::string _name;									// Name of this data ( inside the module defined )
		std::string _helpMessage;							// Help message shown on screen

		DataCreationFunction _creationFunction;				// Function used to create new instances
		DataCompareFunction _compareFunction;				// Function used to compare (when used as key )
		DataSizeFunction _sizeFunction;						// Function used to get the size
		
	public:
		
		/**
		 Inform about the type of operation it is
		 */
		
		Data( std::string name , DataCreationFunction creationFunction , DataCompareFunction compareFunction , DataSizeFunction sizeFunction )
		{
			_name = name;
			_creationFunction = creationFunction;
			_compareFunction = compareFunction;
			_sizeFunction = sizeFunction;
			
			_helpMessage = "Help coming soon";
		}
		
		std::string getName()
		{
			return _name;
		}
		
		DataInstance * getInstance()
		{
			return _creationFunction();
		}
		
		std::string help()
		{
			return _helpMessage;
		};									
	
		DataSizeFunction getSizeFunction()
		{
			 return _sizeFunction;
		}
		
	};
		
	
	
	
} // ss namespace

#endif
