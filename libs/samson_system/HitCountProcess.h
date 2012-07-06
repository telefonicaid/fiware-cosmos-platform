#ifndef _H_SAMSON_system_HIT_COUNT_PROCESS
#define _H_SAMSON_system_HIT_COUNT_PROCESS

#include "samson_system/ProcessComponent.h"

namespace samson{ namespace system{

    class HitCountProcess : public ProcessComponent
    {
        double time_span_;
        double forgetting_factor_;
        size_t n_top_items_;

        std::string out_app_name_;
        std::string out_def_name_;

    public:

        HitCountProcess( ): ProcessComponent("default")
        {
            time_span_ = 300; // 5 minutes
            forgetting_factor_ = (time_span_ - 1) / time_span_;
            n_top_items_ = 100;

            out_app_name_ = "def_out";
            out_def_name_ = "def_def";
        };

        HitCountProcess( double time_span, int n_top_items, std::string out_app, std::string out_def, std::string name ): ProcessComponent(name)
        {
            time_span_ = time_span;
            forgetting_factor_ = (time_span_ - 1) / time_span_;
            n_top_items_ = n_top_items;

            out_app_name_ = out_app;
            out_def_name_ = out_def;

        }

        std::string out_app_name() { return out_app_name_; }
        std::string out_def_name() { return out_def_name_; }

        double update_count(double old_count, size_t old_time, size_t new_time);

        // Update this state based on input values ( return true if this state has been updated with this component )
        bool update( Value* key ,
                            Value * state ,
                            Value ** values ,
                            size_t num_values ,
                            samson::KVWriter *writer );
    };

} } // End of namespace

#endif
