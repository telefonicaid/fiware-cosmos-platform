
#ifndef _H_SAMSON_RATE
#define _H_SAMSON_RATE



#include "au/Rate.h"

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"

namespace samson {
class Rate {
  au::rate::Rate rate_kvs;          // Rate for key values
  au::rate::Rate rate_size;         // Rate for size

public:


  Rate();

  void push(size_t kvs, size_t size);
  void push(FullKVInfo info);

  size_t get_total_size();
  size_t get_total_kvs();
  double get_rate_size();
  double get_rate_kvs();
};
}


#endif  // ifndef _H_SAMSON_RATE

