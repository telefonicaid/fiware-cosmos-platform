
#include "common.h" // Own interface



namespace samson {
    namespace connector {
        
        const char * str_ConnectionType( ConnectionType type )
        {
            switch (type) {
                case connection_input:
                    return "Input";
                case connection_output:
                    return "Output";
            }
            return "Unknown";
        }

    }
}
