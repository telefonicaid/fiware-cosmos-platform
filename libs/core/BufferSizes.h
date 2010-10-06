#ifndef SAMSON_BUFFER_SIZE_H
#define SAMSON_BUFFER_SIZE_H

#define SS_SIZE_1M      1024*1024
#define SS_SIZE_10M     10*1024*1024
#define SS_SIZE_50M     50*1024*1024
#define SS_SIZE_100M    100*1024*1024
#define SS_SIZE_200M    256*1024*1024
#define SS_SIZE_500M    512*1024*1024
#define SS_SIZE_1G      1024*1024*1024
#define SS_SIZE_2G      (size_t)(2)*SIZE_1G
#define SS_SIZE_10G     (size_t)(10)*SIZE_1G

#define MAX_SIZE_KEY_VALUE              SS_SIZE_100M            // Maximum size of the key-value pair (for streaming reasons)
#define SAMSON_MAX_KVSET_SIZE           16*1024*1024

#endif
