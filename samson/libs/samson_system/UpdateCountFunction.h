#ifndef _H_SAMSON_SYSTEM_UPDTE_COUNT_FUNCTION
#define _H_SAMSON_SYSTEM_UPDTE_COUNT_FUNCTION

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
    , time_span_(time_span) // 5 minutes
    , forgetting_factor_((time_span_ - 1) / time_span_)
    , n_top_items_(n_top_items) {
    }

    ~UpdateCountFunction();

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

#endif // ifndef _H_SAMSON_SYSTEM_UPDTE_COUNT_FUNCTION
