
#include "gtest/gtest.h"

#include "au/ThreadManager.h"

#include "engine/MemoryManager.h"

#include "xmlparser/xmlParser.h"

#include "samson/client/SamsonClient.h"

#include "unitTest/common_engine_test.h"

#include "samson/common/ports.h"           // for SAMSON_WORKER_PORT

/*
 *
 * TEST(samson_client, test_1 )
 * {
 *  int port = SAMSON_WORKER_PORT;
 *  char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
 *  if (env_port != NULL)
 *  {
 *      port = atoi(env_port);
 *  }
 *
 * // SamsonClient to play with..
 * samson::SamsonClient* samson_client = init_samson_client_test();
 * LM_M(("samson_client in test_1 initialised"));
 *
 * // Connect to samsonWorker
 * au::ErrorManager error;
 *
 * LM_M(("samson_client in test_1 connected as anonymous"));
 *
 * EXPECT_EQ( error.isActivated() , false) << "Error connecting samsonClient to samsonWorker";
 *
 * // Close samson client test
 * LM_M(("samson_client in test_1 ready to close client"));
 * close_samson_client_test( samson_client );
 * LM_M(("samson_client in test_1 closed client"));
 *
 * }
 *
 * TEST(samson_client, test_2 )
 * {
 *  int port = SAMSON_WORKER_PORT;
 *  char *env_port = getenv("SAMSON_WORKER_PORT_ENV");
 *  if (env_port != NULL)
 *  {
 *      port = atoi(env_port);
 *  }
 *
 * // SamsonClient to play with..
 * samson::SamsonClient* samson_client = init_samson_client_test();
 * LM_M(("samson_client in test_2 initialised"));
 *
 * // Connect to samsonWorker
 * au::ErrorManager error;
 * samson_client->initConnection( &error , "localhost" , port , "anonymous" , "anonymous" );
 * LM_M(("samson_client in test_2 connected as anonymous"));
 *
 * ASSERT_TRUE(samson_client->connection_ready()) << "Connection not ready";
 *
 * // Close samson client test
 * LM_M(("samson_client in test_2 ready to close client"));
 * close_samson_client_test( samson_client );
 * LM_M(("samson_client in test_2 closed client"));
 * }
 *
 */
