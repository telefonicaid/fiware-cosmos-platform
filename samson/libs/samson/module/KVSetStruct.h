#ifndef SAMSON_KV_SET_STRUCT_H
#define SAMSON_KV_SET_STRUCT_H

/* ****************************************************************************
 *
 * FILE               KVSetStruct.h - basic type to work with key-values
 *
 *
 *
 *
 */
#include <sys/types.h>           /* size_t                                   */

#include <samson/module/KV.h>    /* KV                                       */



namespace samson {
typedef struct KVSetStruct {
  KV **kvs;                      // Vector containing Key-values (pointers to row data and hash code)
  size_t num_kvs;                // Number of key-values

  size_t kvs_with_equal_value(size_t pos) {
    if (pos >= num_kvs) {
      return 0;
    }

    for (size_t i = pos + 1; i < num_kvs; i++) {
      if (!kvs[i]->equal_value(kvs[pos])) {
        return i - pos;
      }
    }

    return num_kvs - pos;
  }
} KVSetStruct;
}

#endif // ifndef SAMSON_KV_SET_STRUCT_H
