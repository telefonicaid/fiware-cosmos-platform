
#ifndef _H_DELILAH_COMPONENT
#define _H_DELILAH_COMPONENT

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/Cronometer.h"          // au::CronometerSystem

#include <cstring>
#include "samson/network/Message.h"		// Message::MessageCode 
#include "samson/network/Packet.h"			// samson::Packet


namespace samson {
	
	class Delilah;
	
	class DelilahComponent
	{

		bool component_finished;		// Flag to be removed when indicated by user

	public:
        
        au::CronometerSystem  cronometer;
		
		typedef enum 
		{
			command,                // Command send to controller
			load,
			updater,
            push,
            pop,
            worker_command          // Command send to each worker
		}DelilaComponentType;
		
		DelilaComponentType type;
		
		size_t id;                      // Identifier of this component
		Delilah *delilah;               // Pointer to delilah to notify finish or show messages
        std::string concept;            // Concept of this component to print list of components
        double progress;

        au::ErrorManager error;         // Manager of the error in this operation
        
		DelilahComponent( DelilaComponentType _type);
		virtual ~DelilahComponent(){};	// Virtual destructor necessary in this class since subclasses are deleted using parent pointers
		
		void setId( Delilah * _delilah ,  size_t _id );
		virtual void receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;

		// General function to give a long description status ( used when typing ps X )
		virtual std::string getStatus()=0;

        // Check if the component is finished
        bool isComponentFinished();
        
        // A string description of the type of operation
        std::string getTypeName();

        // One line description to show in lists
        std::string getDescription();
        std::string getIdAndConcept();
        
    protected:
        
        void setConcept( std::string _concept );
        void setProgress( double p );
        
        
    protected:
        
        void setComponentFinished();
        void setComponentFinishedWithError( std::string error_message );
        
	};
	
}

#endif