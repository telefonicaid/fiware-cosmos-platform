#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

/* ****************************************************************************
 *
 * FILE                     NetworkInterface.h - network interface
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */
#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // LmtNetworkInterface, ...

#include "au/ErrorManager.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/string/string.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

#include "samson/network/Message.h"               // samson::Message::MessageCode
#include "samson/network/Packet.h"

#endif  // ifndef _H_NETWORK_INTERFACE
