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

/*
 * FILE            UpdateCountFunction.h
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Declaration of UpdateCountFunction class to update a time weighted count
 *  This class allows to keep different term profiles in HitCountProcess
 *
 */

#ifndef _H_SAMSON_SYSTEM_UPDATE_COUNT_FUNCTION
#define _H_SAMSON_SYSTEM_UPDATE_COUNT_FUNCTION

#include <cstring>  //size_t
#include <string>

namespace samson {
namespace system {
class UpdateCountFunction {


  public:
    UpdateCountFunction()
    : name_("def_update")
    , time_span_(300.0) // 5 minutes
    , forgetting_factor_((time_span_ - 1) / time_span_)
    , n_top_items_(100) {
    }

    UpdateCountFunction(const std::string& name, double time_span, int n_top_items)
    : name_(name)
    , time_span_(time_span)
    , forgetting_factor_((time_span_ - 1) / time_span_)
    , n_top_items_(n_top_items) {
    }

    ~UpdateCountFunction() {
    }

    std::string name() const {
      return name_;
    }

    size_t n_top_items() const {
      return n_top_items_;
    }

    double UpdateCount(double old_count, size_t old_time, size_t new_time);

  private:
    std::string name_;
    double time_span_;
    double forgetting_factor_;
    size_t n_top_items_;
};
}
} // End of namespaces

#endif // ifndef _H_SAMSON_SYSTEM_UPDATE_COUNT_FUNCTION
