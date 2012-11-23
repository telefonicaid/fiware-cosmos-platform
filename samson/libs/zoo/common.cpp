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

#include "common.h"  // Own interface
#include <string.h>

namespace au {
  namespace zoo {
    std::string str_error(int rc) {
      switch (rc) {
        case ZC_ERROR_GPB:
          return "GoogleProtocolBuffer serialization error";
          
          break;
          
        case ZC_ERROR_NO_CONNECTION:
          return "No connection stablished";
          
          break;
          
        case ZC_ERROR_CONNECTION_TIMEOUT:
          return "Connection timeout";
          
          break;
          
        case ZC_ERROR_GPB_NO_INITIALIZED:
          return "GoogleProtocolBuffer missing required fields";
          
        default:
          return zerror(rc);
          
          break;
      }
    }
    
    int vstrcmp(const void *str1, const void *str2) {
      const char **a = (const char **)str1;
      const char **b = (const char **)str2;
      
      return strcmp(*a, *b);
    }
    
    void sort_vector(struct String_vector *vector) {
      qsort(vector->data, vector->count, sizeof(char *), &vstrcmp);
    }
    
    void free_vector(struct String_vector *v) {
      if (v->data) {
        int32_t i;
        for (i = 0; i < v->count; i++) {
          free(v->data[i]);
        }
        free(v->data);
        v->data = 0;
      }
    }
    
    void init_vector(struct String_vector *v) {
      v->count = 0;
      v->data = NULL;
    }
  }
}  // end of namespace samson::zoo

