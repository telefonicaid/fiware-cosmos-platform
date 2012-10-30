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

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_passive_location_Record
#define _H_SAMSON_passive_location_Record


#include <samson/modules/passive_location/Record_base.h>


namespace samson{
namespace passive_location{


	class Record : public Record_base
	{
        std::string str(){
                std::ostringstream o;
                                { //Texting imsi
                        o << imsi.str();
                }

                o<<" ";
                                { //Texting imei
                        o << imei.str();
                }

                o<<" ";
                                { //Texting msisdn
                        o << msisdn.str();
                }

                o<<" ";
                                { //Texting timestamp
                        o << timestamp.str();
                }

                o<<" ";
                                { //Texting cellId
			o << cellId.str() << "(0x" << std::hex << (cellId.value >> 16) << ",0x" << (cellId.value & 0x0000ffff) << ")(" << std::dec << (cellId.value >> 16) << "," << (cellId.value & 0x0000ffff) << ")";

                }

                o<<" ";
                                { //Texting callType
                        o << callType.str();
                }

                o<<" ";
                                { //Texting DRType
                        o << DRType.str();
                }

                o<<" ";
                return o.str();
        }

	};


} // end of namespace samson
} // end of namespace passive_location

#endif
