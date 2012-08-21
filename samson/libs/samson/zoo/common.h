
#ifndef _H_ZOOKEEPER_COMMON
#define _H_ZOOKEEPER_COMMON

#include "zookeeper/zookeeper.h"
#include <string>

// Additional errors of the C++ zookeeper wrapper

#define ZC_ERROR_GPB                -2000    // New error codes in this zoo API C++ wrapper
#define ZC_ERROR_GPB_NO_INITIALIZED -2001    // NO all required fields are set
#define ZC_ERROR_NO_CONNECTION      -2002    // No connection is established
#define ZC_ERROR_CONNECTION_TIMEOUT -2003    // No connection is established

namespace samson { namespace zoo {
                   class Connection;

                   // Function to get the error message in zk library
                   std::string str_error(int rc);

                   // Auxiliar functions to work with
                   int vstrcmp(const void *str1, const void *str2);
                   void sort_vector(struct String_vector *vector);
                   void free_vector(struct String_vector *v);
                   void init_vector(struct String_vector *v);
                   } } // End of namespace samson::zoo

#endif // ifndef _H_ZOOKEEPER_COMMON
