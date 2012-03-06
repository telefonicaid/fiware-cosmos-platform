

#include <stdio.h>
#include "au/string.h"

#include <samson/module/DataInstance.h>
#include <samson/modules/system/Value.h>

#include "operations/Filter.h"
#include "operations/map_value_to_value.h"



int main()
{
    
    samson::system::FilterSelect filter_select( "[key:0;key:1],<andreu>" );
    std::cout << filter_select.str();
}
