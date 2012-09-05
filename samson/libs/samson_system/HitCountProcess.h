#ifndef _H_SAMSON_system_HIT_COUNT_PROCESS
#define _H_SAMSON_system_HIT_COUNT_PROCESS

#include <string>
#include <vector>

#include "samson_system/ProcessComponent.h"
#include "samson_system/UpdateCountFunction.h"

namespace samson {
namespace system {
class HitCountProcess : public ProcessComponent {

  public:
    static const std::string kNullDest;

    HitCountProcess() :
      ProcessComponent("default") {
      out_app_name_ = "def_out";
      out_def_name_ = "def_def";
    }

    HitCountProcess(const std::string& name, const std::string& out_app, const std::string& out_def) :
      ProcessComponent(name) {
      out_app_name_ = out_app;
      out_def_name_ = out_def;
    }

    ~HitCountProcess() {
      counts_.clear();
    }

    std::string out_app_name() {
      return out_app_name_;
    }

    std::string out_def_name() {
      return out_def_name_;
    }

    void AddUpdateCountFunction(const std::string& name, double time_span, int n_top_items);

    // Update this state based on input values ( return true if this state has been updated with this component )
    bool Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);

  private:
    std::vector<UpdateCountFunction> counts_;
    std::string out_app_name_;
    std::string out_def_name_;

};
}
} // End of namespace

#endif // ifndef _H_SAMSON_system_HIT_COUNT_PROCESS
