
namespace samson {
class SamsonClient;
}



// Init and close methods for all engine tests
void init_engine_test();
void close_engine_test();


// Init and close methods for test with samson client
samson::SamsonClient *init_samson_client_test();
void close_samson_client_test(samson::SamsonClient *samson_client);
