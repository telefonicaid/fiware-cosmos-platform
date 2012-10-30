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
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_passive_location_update_tmsi_imsi_table_with_new_tmsi
#define _H_SAMSON_passive_location_update_tmsi_imsi_table_with_new_tmsi


#include <samson/module/samson.h>
#include <samson/modules/passive_location/CompleteTMSI.h>
#include <samson/modules/passive_location/IMSIbyTime.h>


namespace samson{
namespace passive_location{


class update_tmsi_imsi_table_with_new_tmsi : public samson::Reduce
{

    samson::passive_location::CompleteTMSI completeTMSI;    // Input key
    samson::passive_location::CompleteTMSI newTMSI;    // Used as value at the output
    samson::passive_location::CompleteTMSI oldTMSI;    // Input key for TMSI update
    samson::passive_location::IMSIbyTime newIMSI;                           // Input value of the records
    samson::passive_location::IMSIbyTime stateIMSI;                         // Input value of the state

    uint64_t  tmps_timespan;
    time_t now;

public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  input: passive_location.CompleteTMSI passive_location.CompleteTMSI
    //  input: passive_location.CompleteTMSI passive_location.IMSIbyTime
    //  output: passive_location.CompleteTMSI passive_location.IMSIbyTime
    //
    // helpLine: Update the table to translate to recover imsi from tmsi, with oldTMSI/newTMSI pairs
    //  END_INFO_MODULE

    void init( samson::KVWriter *writer )
    {
        tmps_timespan = environment->get("pl.tmps_age.timespan", 0);
        if ( tmps_timespan != 0)
        {
            now = time(NULL);
        }
    }

    void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {

        if (inputs[1].num_kvs > 0)
        {
            // We don't have an IMSIstate whose TMSI should be updated
            return;
        }

        if (inputs[0].num_kvs == 0)
        {
            // If we don't receive new tmsi, just resend the state

            // If we are in 'live mode' (timespan != 0), only reemit recent
            // associations
            if (inputs[1].num_kvs > 1)
            {
                LM_M(("More than one(%d) tmsi-imsi record in table, without update", inputs[1].num_kvs));
            }

            // In any case, we will just re-emit the last state
            stateIMSI.parse(inputs[1].kvs[inputs[1].num_kvs-1]->value);
            if ((tmps_timespan == 0) || ((now - stateIMSI.timestamp.value) < (int64_t)tmps_timespan))
            {
                //for (uint64_t i = 0; (i < inputs[1].num_kvs); i++)
                {
                    completeTMSI.parse(inputs[1].kvs[0]->key);
                    writer->emit( 0 , &completeTMSI , &stateIMSI );
                }
            }
            return;
        }

        // We have a TMSI update
        // We emit the new TMSI, with the same IMSI
        // (the old TMSI will be reemited in next loops)
        // We keep also the old TMSI-IMSi association, because
        // we could receive records just from before and after this one
        // We know that we lose the timespan of this TMSI update

        // If we are in 'live mode' (timespan != 0), only reemit recent
        // associations

        // In any case, we will just re-emit the last state
        stateIMSI.parse(inputs[1].kvs[inputs[1].num_kvs-1]->value);

        if ((tmps_timespan == 0) || ((now - stateIMSI.timestamp.value) < (int64_t)tmps_timespan))
        {
            // We just emit one. We are not sure it is the last one by time, but there is no solution
            // with the data-types we are using now
            newTMSI.parse(inputs[0].kvs[inputs[0].num_kvs-1]->value);
            writer->emit( 0 , &newTMSI , &stateIMSI );

        }
    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace passive_location
} // end of namespace samson

#endif
