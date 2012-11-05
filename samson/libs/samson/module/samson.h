

#ifndef _H_SAMSON_SAMSON
#define _H_SAMSON_SAMSON

/**
 *
 * Main file for module development
 *
 */


#include "au/log/LogMain.h" // Now it is mandatory compile libraries including au library

#include <samson/module/Data.h>                /* Data                                     */
#include <samson/module/DataInstance.h>        /* DataInstance                             */
#include <samson/module/Factory.h>             /* au::factory                              */
#include <samson/module/KVFormat.h>
#include <samson/module/KVWriter.h>
#include <samson/module/Module.h>
#include <samson/module/Operation.h>
#include <samson/module/samsonTypes.h>         /* ss_*                                     */
#include <samson/module/samsonVersion.h>       /* SAMSON_VERSION                           */
#include <samson/module/var_int.h>             /* Static functions for var int codification */

namespace samson {
  extern int log_operations;
}

// MACROS USED TO EMIT LOGS TO "log_operations" channels

#define AU_OE(s) AU_E( ::samson::log_operations , s )
#define AU_OW(s) AU_W( ::samson::log_operations , s )
#define AU_OM(s) AU_M( ::samson::log_operations , s )
#define AU_OD(s) AU_D( ::samson::log_operations , s )

#endif
