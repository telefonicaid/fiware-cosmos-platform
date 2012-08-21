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
#include "au/string.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"

#include "samson/network/Message.h"               // samson::Message::MessageCode
#include "samson/network/Packet.h"

namespace samson {
/* ****************************************************************************
 *
 * NetworkInterfaceReceiver -
 */

class NetworkInterfaceReceiver {
public:

  // Method implemented to process received packets
  virtual void receive(const PacketPointer& packet) = 0;

  // Virtual destructor necessary since pure virtual methods are defined
  virtual ~NetworkInterfaceReceiver() {
  };
};
}

#endif // ifndef _H_NETWORK_INTERFACE
