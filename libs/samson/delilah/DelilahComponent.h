
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
            worker_command,          // Command send to each worker
            repeat                   // Repeater task
		}DelilaComponentType;
		
		DelilaComponentType type;
		
		size_t id;                      // Identifier of this component
		Delilah *delilah;               // Pointer to delilah to notify finish or show messages
        std::string concept;            // Concept of this component to print list of components

        double progress;
        bool hidden;       // Flag to not show information on screen about this

        au::ErrorManager error;         // Manager of the error in this operation
        
		DelilahComponent( DelilaComponentType _type);
		virtual ~DelilahComponent(){};	// Virtual destructor necessary in this class since subclasses are deleted using parent pointers
		
		void setId( Delilah * _delilah ,  size_t _id );
		virtual void receive( Packet* packet )=0;

		// General function to give a long description status ( used when typing ps X )
		virtual std::string getStatus()=0;

        // Check if the component is finished
        bool isComponentFinished();
        
        // A string description of the type of operation
        std::string getTypeName();

        // One line description to show in lists
        std::string getStatusDescription();
        std::string getIdAndConcept();
        
        
        // Short description
        virtual std::string getShortDescription()
        {
            std::ostringstream output;
            output << "[" << id << "]";
            return output.str();
        }
        
        std::string getIdStr()
        {
            std::ostringstream output;
            output << id;
            return output.str();
        }
        
    protected:
        
        void setConcept( std::string _concept );
        void setProgress( double p );
        
        
    protected:
        
        void setComponentFinished();
        void setComponentFinishedWithError( std::string error_message );
        
	};
    
    
    class RepeatDelilahComponent : public DelilahComponent , engine::Object
    {
        std::string command;
        int seconds;
        
        au::Cronometer cronometer;
        
    public:
      
        RepeatDelilahComponent( std::string _command , int _seconds );
		std::string getStatus();
        void notify( engine::Notification* notification );

		void receive( Packet* packet )
        {
            LM_W(("Received a packet in a RepeatDelilahComponent... nothing was expected"));
        }
        
        void run();
        
    };
	
}

#endif