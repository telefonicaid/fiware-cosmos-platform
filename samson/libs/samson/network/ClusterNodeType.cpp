/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "logMsg/logMsg.h"

#include "ClusterNodeType.h" // Own interface

namespace samson
{
   
   const char* ClusterNodeType2str( ClusterNodeType type )
   {
	  switch ( type )
	  {
	  case DelilahNode: return "delilah";
	  case WorkerNode:  return "worker";
	  case UnknownNode: return "unknown";
	  }
	  
	  LM_E(("Impossible to got here with type:%d", type));
	  return "Error";
   }
   
}
