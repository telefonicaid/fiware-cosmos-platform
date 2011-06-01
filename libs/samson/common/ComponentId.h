#ifndef COMPONENT_ID
#define COMPONENT_ID


namespace samson
{



/* ****************************************************************************
*
* ComponentId - 
*/
class ComponentId
{
public:
	typedef enum Type
	{
		NoType,
		Controller,
		Worker
	} Type;

	Type   type;
	size_t id;

	ComponentId()
	{
		type = NoType;
		id   = 0;
	} 

	ComponentId( Type _type, size_t _id ) 
	{
		type = _type;
		id = _id;
	}

	static ComponentId controller()
	{
		return ComponentId( Controller , 0); 
	}

	static ComponentId worker( int id )
	{
		return ComponentId( Worker , id); 
	}
};

}

#endif
