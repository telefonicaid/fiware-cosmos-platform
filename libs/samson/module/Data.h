#ifndef SAMSON_DATA_H
#define SAMSON_DATA_H


#include <math.h>						/*                                          */
#include <stdlib.h>						/* malloc                                   */
#include <string.h>						/* memcpy                                   */
#include <string>						/* std::string                              */

#include <samson/module/KV.h>                  /* KV                                       */
#include <samson/module/KVVector.h>            /* KVVector                                 */
#include <samson/module/DataInstance.h>        /* DataInstance                             */



namespace samson {
	
	// Static functions necessary for a Data
	typedef void* (*DataCreationFunction)();

	// Function used to get the size of data from the stream of bytes
	typedef int(* DataSizeFunction)(char *data);
	
	// Templatd function to create an instance of DataInstance
	template <class T>
	void* getDataIntace()
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
		DataSizeFunction _sizeFunction;						// Function used to get the size
		
	public:
		
		/**
		 Inform about the type of operation it is
		 */
		
		Data( std::string name , DataCreationFunction creationFunction , DataSizeFunction sizeFunction )
		{
			_name = name;
			_creationFunction = creationFunction;
			_sizeFunction = sizeFunction;
			
			_helpMessage = "Help coming soon";
		}

		Data( Data *d )
		{
			_name = d->_name;
			_creationFunction = d->_creationFunction;
			_sizeFunction = d->_sizeFunction;
			
			_helpMessage = d->_helpMessage;
		}
		
        void getInfo( std::ostringstream& output)
        {
            output << "<data>\n";
            output << "<name>" << _name << "</name>\n";
            output << "<help>" << _helpMessage << "</help>\n";
            output << "</data>\n";
        }
        
		std::string getName()
		{
			return _name;
		}
		
		void * getInstance()
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
