

#include "DelilahComponent.h"		// Own interface
#include "Packet.h"					// ss::Packet
#include "Delilah.h"				// ss::Delilah
#include "EnvironmentOperations.h"	// copyEnviroment()

namespace ss {

	DelilahComponent::DelilahComponent()
	{
		component_finished =  false;
	}
	
	void DelilahComponent::setId( Delilah * _delilah ,  size_t _id )
	{
		delilah = _delilah;
		id = _id;
	}
	
	
	
}

