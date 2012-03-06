

#include <stdio.h>
#include "au/string.h"

#include <samson/module/DataInstance.h>
#include <samson/modules/system/Value.h>

#include "operations/Filter.h"
#include "operations/map_value_to_value.h"



int main()
{

   samson::system::Value v;
   v.set_string("10.4");

   printf("%s\n" , v.str().c_str() );

}
