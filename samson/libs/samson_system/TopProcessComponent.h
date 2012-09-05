#ifndef _H_SAMSON_system_TOP_PROCESS_COMPONENT
#define _H_SAMSON_system_TOP_PROCESS_COMPONENT

#include "samson/module/KVWriter.h"
#include "samson_system/ProcessComponent.h"
#include "samson_system/Value.h"

namespace samson {
namespace system {

class TopProcessComponent : public ProcessComponent {

  public:

    TopProcessComponent() :
      ProcessComponent("top")
      , time_span_(300)
      , top_list_max_size_(100)
      , forgetting_factor_((static_cast<double> (time_span_ - 1)) / (static_cast<double> (time_span_))) {
    }

    static void EmitTopElement(const char *concept, const char *category, double counter, samson::KVWriter* const writer);

    // Update this state based on input values
    bool Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);
    void UpdateAccumulator(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);
    void UpdateCategoryWithValue(Value *state, Value *new_value);
    void UpdateCategory(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);

  private:
    int time_span_;
    unsigned int top_list_max_size_;
    double forgetting_factor_;
};
}
} // End of namespace

#endif  // ifndef _H_SAMSON_system_TOP_PROCESS_COMPONENT
