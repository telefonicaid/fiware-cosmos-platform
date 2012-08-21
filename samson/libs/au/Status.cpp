

#include "Status.h"  // Own interface

namespace au {
const char *status(Status code) {
  switch (code) {
    case OK:                        return "Ok";

    case Error:                     return "Error";

    case NotImplemented:            return "Not implemented";

    case OpenError:                 return "Open error";

    case SelectError:               return "Error in select call";

    case Timeout:                   return "Timeout";

    case ConnectError:              return "Connect error";

    case AcceptError:               return "Accept error";

    case SocketError:               return "Socket error";

    case BindError:                 return "Bind error";

    case ListenError:               return "Listen error";

    case ReadError:                 return "read error";

    case WriteError:                return "write error";

    case ConnectionClosed:          return "connection closed";

    case GetHostByNameError:        return "GetHostByNameError";

    case GPB_Timeout:               return "GPB Error: timeout";

    case GPB_ClosedPipe:            return "GPB Error: closed pipe";

    case GPB_ReadError:             return "GPB Error reading";

    case GPB_CorruptedHeader:       return "GPB Error: corrupted header";

    case GPB_WrongReadSize:         return "GPB Error: wrong read size";

    case GPB_ReadErrorParsing:      return "GPB Error: pasing google protocol message";

    case GPB_NotInitializedMessage: return "GPB Error: Not initialized google procool message ( required fields maybe )";

    case GPB_WriteErrorSerializing: return "GPB Error: serializing google procool message";

    case GPB_WriteError:            return "GPB Write Error";
  }

  return "Unknown";
}
}