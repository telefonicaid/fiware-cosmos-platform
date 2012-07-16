/* ****************************************************************************
 *
 * FILE            PushDelilahComponent_test.cpp
 *
 * AUTHOR         Gregorio Escalada
 *
 * DATE            May 2012
 *
 * DESCRIPTION
 *
 * unit testing of the delilah class in the samson  library
 *
 */

#include "gtest/gtest.h"

#include "samson/common/ports.h" // for SAMSON_WORKER_PORT
#include "samson/delilah/Delilah.h"                                     // samson::Delailh
#include "samson/delilah/DelilahConsole.h"
#include "samson/delilah/PushDelilahComponent.h"

#include "common_delilah_test.h"

#include "unitTest/common_engine_test.h"

//Test void getStatus();
TEST(PushDelilahComponentTest, getStatus)
{
   init_engine_test();
   
   
   int length = 1024;
   char *data = static_cast<char *>(malloc(length));
   strcpy(data, "This is a test");
   samson::BufferDataSource * ds = new samson::BufferDataSource( data , length );
   samson::PushDelilahComponent * d = new samson::PushDelilahComponent( ds , "input_queue" );
   d->addQueue("another_input_queue");
   
   EXPECT_TRUE(strstr(d->getStatus().c_str(), "to queue/s input_queue") != NULL) << "Wrong status from getStatus()";
   EXPECT_TRUE(strstr(d->getShortDescription().c_str(), "[ Id ") != NULL) << "Wrong status from getShortDescription()";
   
   close_engine_test();
}


