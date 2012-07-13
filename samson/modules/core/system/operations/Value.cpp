
#include "samson/modules/system/Value.h" // Own interface


namespace samson 
{
    namespace system 
    {
        // Static pool for object resusage
        au::Pool<Value> samson::system::Value::pool_values;
        
    }
}