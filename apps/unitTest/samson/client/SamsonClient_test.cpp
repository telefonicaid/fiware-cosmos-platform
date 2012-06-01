
#include "gtest/gtest.h"

#include "au/ThreadManager.h"

#include "engine/MemoryManager.h"
#include "engine/MemoryRequest.h"

#include "xmlparser/xmlParser.h"

#include "samson/client/SamsonClient.h"

#include "unitTest/common_engine_test.h"

TEST(samson_client, test_1 )
{
   // SamsonClient to play with..
   samson::SamsonClient* samson_client = init_samson_client_test();

   // Connect to samsonWorker
   au::ErrorManager error;   
   samson_client->initConnection( &error , "localhost" , 1324 , "anonymous" , "anonymous" );

   EXPECT_EQ( error.isActivated() , false) << "Error connecting samsonClient to samsonWorker";

   // Close samson client test
   close_samson_client_test( samson_client );

}    

TEST(samson_client, test_2 )
{

   // SamsonClient to play with..
   samson::SamsonClient* samson_client = init_samson_client_test();

   // Connect to samsonWorker
   au::ErrorManager error;
   samson_client->initConnection( &error , "localhost" , 1324 , "anonymous" , "anonymous" );

   ASSERT_TRUE(samson_client->connection_ready()) << "Connection not ready";

   // Close samson client test
   close_samson_client_test( samson_client );
}
