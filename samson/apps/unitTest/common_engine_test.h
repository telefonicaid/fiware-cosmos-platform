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

namespace samson {
class SamsonClient;
}



// Init and close methods for all engine tests
void init_engine_test();
void close_engine_test();


// Init and close methods for test with samson client
samson::SamsonClient *init_samson_client_test();
void close_samson_client_test(samson::SamsonClient *samson_client);
