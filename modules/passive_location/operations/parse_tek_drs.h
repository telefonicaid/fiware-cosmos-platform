
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_passive_location_parse_tek_drs
#define _H_SAMSON_passive_location_parse_tek_drs


#include <arpa/inet.h> // ntohl(), ntohs()
#include <samson/module/samson.h>
#include <samson/modules/passive_location/Record.h>
#include <samson/modules/passive_location/CompleteTMSI.h>
#include <samson/modules/system/UInt.h>

#include "logMsg/logMsg.h"

#include "tektronix_data.h"


namespace samson{
namespace passive_location{


class parse_tek_drs : public samson::Parser
{

    samson::system::UInt user;                              // Used as key at the output
    samson::passive_location::Record record;                // Used as value at the output
    samson::passive_location::CompleteTMSI completeTMSI;    // Used as value at the output
    samson::passive_location::IMSIbyTime imsiTime;    // Used as value at the output

public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    // out system.UInt passive_location.Record                     # Emitted with key = imsi
    // out passive_location.CompleteTMSI passive_location.Record   # Whem imsi = 0, emitted with key = tmsi
    // out system.UInt passive_location.Record                     # If no cellId or LAC is parsed in the record
    // out passive_location.CompleteTMSI passive_location.IMSIbyTime               # Queue to recover imsi from tmsi

    //
    // helpLine: Parse input binary OHDRs from Tektroniks platform (probes). Note that output key is cellid
    //  END_INFO_MODULE

    void init( samson::KVWriter *writer )
    {
    }

    void run( char *data , size_t length , samson::KVWriter *writer )
    {
        size_t offset = 0;

        //OLM_M(("length:%lu", length));

        unsigned char *p_blob = (unsigned char *)data;
        unsigned char *p_end_blob = (unsigned char *)data + length;

        unsigned char *p_init_ohdr;
        unsigned char *p_end_ohdr;

        unsigned int sizeOHDR = 0;
        int typeMsg = 0;
        int numDRs = 0;

        unsigned int sizeDR = 0;
        struct struct_tek_record tek_record;



        for (int i = 0; (i < 232); i++)
        {
            //OLM_M(("data[%d](0x%0x) = 0x%0x", i, p_blob+i, int(p_blob[i])));
        }

        while( p_blob < p_end_blob )
        {
            p_init_ohdr = p_blob;
            if (parse_OHDR_header(&p_blob, &sizeOHDR, &numDRs, &typeMsg))
            {
                p_end_ohdr = p_init_ohdr + sizeOHDR;
                for (int i = 0; ((i < numDRs) && (p_blob < p_end_ohdr)); i++)
                {
                    init_tek_record(&tek_record);
                    if (parse_DR(&p_blob, &sizeDR, &tek_record))
                    {

                        completeTMSI.tmsi.value = tek_record.tmsi;
                        completeTMSI.LAC.value = tek_record.LAC;

                        user.value = tek_record.imsi;

                        record.imsi.value = tek_record.imsi;
                        record.imei.value = tek_record.imei;
                        record.timestamp.value = tek_record.timestamp;
                        // We compose location id with ((LAC << 16) | cell_id), in a uint32_t field
                        uint32_t cellIdTmp = tek_record.LAC;
                        record.cellId.value = (cellIdTmp << 16) | tek_record.cellID;
                        record.callType.value = tek_record.callType;
                        record.DRType.value = tek_record.typeDR;

                        LM_M(("Ready to emit typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu tmsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.tmsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));

                        // Emit the record at the corresponding output
                        if ((tek_record.cellID == 0) || (tek_record.LAC == 0))
                        {
                            LM_W(("Emit records without cellId: typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));
                            writer->emit(2, &user, &record);
                        }
                        else if (tek_record.imsi == 0)
                        {
                            LM_W(("Emit records without imsi: typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu tmsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.tmsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));

                            writer->emit(1, &completeTMSI, &record);
                        }
                        else
                        {
                            LM_W(("Emit complete records: typeDR:%d for callNumber:%d callType:0x%0x imsi:%lu imei:%lu at cellId:%d in LAC:%d (compose:%lu 0x%0x) at %lu(%s)", tek_record.typeDR, tek_record.callNumber, tek_record.callType, tek_record.imsi, tek_record.imei, tek_record.cellID, tek_record.LAC, record.cellId.value, record.cellId.value, record.timestamp.value, record.timestamp.str().c_str()));
                           writer->emit(0, &user, &record);
                        }

                        if ((tek_record.imsi != 0) && (tek_record.LAC != 0))
                        {
                            imsiTime.imsi.value = user.value;
                            imsiTime.timestamp.value = tek_record.timestamp;

                            writer->emit(3, &completeTMSI, &imsiTime);
                        }

                        free(tek_record.CCCause);
                        free(tek_record.MMCause);
                        free(tek_record.RANAPCause);
                        free(tek_record.ALCAPCause);
                    }
                }
                if (p_blob != p_end_ohdr)
                {
                    LM_W(("Alignment failed in a OHDR of %d DRs", numDRs));
                }
            }
            else
            {
                LM_W(("OHDR ignored because not valid header, with typeMsg=%d", typeMsg));
            }
            //OLM_M(("p_end_blob - p_blob=%lu (length(%lu))", p_end_blob - p_blob, length));
        }
    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace passive_location
} // end of namespace samson

#endif
