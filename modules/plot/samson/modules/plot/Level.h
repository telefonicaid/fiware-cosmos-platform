
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_plot_Level
#define _H_SAMSON_plot_Level


#include <samson/modules/plot/Level_base.h>

#include <samson/modules/plot/LevelUpdate.h>

#include "common.h"

namespace samson{
namespace plot{


	class Level : public Level_base
	{

	public:
	   
	   void init()
	   {
		  // Init value
		  level.value = 0;		  
		  published_level.value = 0;
	   }
	   
	   void set_published()
	   {
		  published_level.value = level.value;
	   }

	   bool changed_significantly()
	   {
		  if( level.value == published_level.value )
			 return false;

		  if( published_level.value == 0)
			 return true;

		  double diff  = fabs( published_level.value - level.value ); 
		  if( ( diff / published_level.value ) > 0.1 )
			 return true;

		  return false;
	   }

	   void update( LevelUpdate *update )
	   {
		   switch( update->command.value )
		   {
		   case plot_level_set:
			   level.value = update->level.value;
			   return;
		   case plot_level_increment:
			   level.value += update->level.value;
			   return;
		   case plot_level_decrement:
			   level.value -= update->level.value;
			   return;			   
		   }		   
	   
	   }

	   
	};


} // end of namespace samson
} // end of namespace plot

#endif
