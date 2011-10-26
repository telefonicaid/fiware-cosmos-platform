
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_simple_mobility_UserArea
#define _H_SAMSON_simple_mobility_UserArea


#include <samson/modules/simple_mobility/UserArea_base.h>
#include <samson/modules/simple_mobility/Position.h>


namespace samson{
namespace simple_mobility{


	class UserArea : public UserArea_base
	{
	public:

	   bool isInside( samson::simple_mobility::Position *position)
	   {
		  // Check if a particular position is inside or not
		  double x_dif = position->x.value - x.value;
		  double y_dif = position->y.value - y.value;

		  double distance = sqrt(x_dif*x_dif + y_dif*y_dif );

		  if( distance < radius.value )
			 return true;
		  else
			 return false;
		  

	   }

	};


} // end of namespace samson
} // end of namespace simple_mobility

#endif
