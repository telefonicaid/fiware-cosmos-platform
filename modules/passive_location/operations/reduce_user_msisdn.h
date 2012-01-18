
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_passive_location_reduce_user_msisdn
#define _H_SAMSON_passive_location_reduce_user_msisdn


#include <samson/module/samson.h>
#include <samson/modules/passive_location/Record.h>
#include <samson/modules/passive_location/User.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace passive_location{


class reduce_user_msisdn : public samson::Reduce
{
    samson::system::UInt imsi_key;                // Input key
    samson::passive_location::Record record;      // Input & Output record
    samson::passive_location::User userInfo;      // Information about cell

public:


    //  INFO_MODULE
    // If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
    // Please, do not remove this comments, as it will be used to check consistency on module declaration
    //
    //  input: system.UInt passive_location.Record
    //  input: system.UInt passive_location.User
    //  output: system.UInt passive_location.Record
    //  output: system.UInt passive_location.Record
    //
    // helpLine: Extract msisdn info from user table
    //  END_INFO_MODULE

    void init( samson::KVWriter *writer )
    {
    }

    void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
    {
        if (inputs[0].num_kvs == 0)
        {
            return;
        }

        if( inputs[1].num_kvs == 0)
        {
            imsi_key.parse(inputs[0].kvs[0]->key);
            LM_W(("No userInfo for imsi:%lu (%lu records)", imsi_key.value, inputs[0].num_kvs));

            // Non existing user... forward input to output 1
            for ( uint64_t i = 0 ; i< inputs[0].num_kvs ; i++)
            {
                record.parse( inputs[0].kvs[i]->value );
                writer->emit( 1 , &record.imsi , &record );
            }
            return;
        }

        imsi_key.parse(inputs[1].kvs[0]->key);

        if (inputs[1].num_kvs > 1)
        {
            LM_W(("More than one user description for imsi:%lu (%lu infos)", imsi_key.value, inputs[1].num_kvs));
        }
        LM_M(("Emitting %lu records for imsi:%lu", inputs[0].num_kvs, imsi_key.value));


        userInfo.parse( inputs[1].kvs[0]->value ); // Only consider the first one...

        for ( uint64_t i = 0 ; i< inputs[0].num_kvs; i++)
        {
            record.parse( inputs[0].kvs[i]->value );

            // Complete the complete record
            record.msisdn.value = userInfo.msisdn.value;

            writer->emit( 0 , &record.msisdn , &record );
        }
        return;

    }

    void finish( samson::KVWriter *writer )
    {
    }



};


} // end of namespace passive_location
} // end of namespace samson

#endif