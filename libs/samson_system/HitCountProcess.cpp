
#include "samson_system/ValueContainer.h"

#include "HitCountProcess.h" // Own interface


namespace samson{ namespace system{

double HitCountProcess::update_count(double old_count, size_t old_time, size_t new_time)
{
    int diff = new_time - old_time;
    return (old_count * pow(forgetting_factor_, diff ));
}

bool HitCountProcess::update( Value* key ,Value * state ,Value ** values ,size_t num_values ,samson::KVWriter *writer )
{

    Value *p_hit;

    std::string concept;
    std::string item;

    size_t newest_timestamp = 0;

    if (key->checkMapValue("app", name().c_str()))
    {
        if (state->isVoid())
        {
            state->set_as_vector();
        }

        for (size_t i = 0; (i < num_values); i++)
        {
            item = values[i]->get_string_from_map("item");
            size_t timestamp = values[i]->get_double_from_map("time");
            if (timestamp > newest_timestamp)
            {
                newest_timestamp = timestamp;
            }
            Value *p_hit = NULL;
            for (size_t j = 0; (j < state->get_vector_size()); j++)
            {
                p_hit = state->get_value_from_vector(j);
                if (concept == p_hit->get_string_from_map("concept"))
                {
                    double old_count = p_hit->get_double_from_map("count");
                    size_t old_timestamp = p_hit->get_double_from_map("time");
                    double new_count = 1 + update_count(old_count, old_timestamp, timestamp);
                    p_hit->set_double_for_map("time", timestamp);
                    p_hit->set_double_for_map("count", new_count);
                    break;
                }
            }
            if (p_hit == NULL)
            {
                Value *new_hit = state->add_value_to_vector(state->get_vector_size());
                new_hit->set_as_map();
                new_hit->add_value_to_map("concept")->set_string(concept);
                new_hit->add_value_to_map("time")->set_double(timestamp);
                new_hit->add_value_to_map("count")->set_double(1);
            }
        }

        for (size_t j = 0; (j < state->get_vector_size()); j++)
        {
            p_hit = state->get_value_from_vector(j);
            double old_count = p_hit->get_double_from_map("count");
            size_t old_timestamp = p_hit->get_double_from_map("time");
            double new_count = update_count(old_count, old_timestamp, newest_timestamp);
            p_hit->set_double_for_map("time", newest_timestamp);
            p_hit->set_double_for_map("count", new_count);
        }

        // simple bubble sort implementation. Difficult to use sort() with Value
        bool swapped = true;
        for (size_t i = 0; ((i < state->get_vector_size()) && (swapped = true)); i++)
        {
            swapped = false;
            for (size_t j = 0; (j < state->get_vector_size()); j++)
            {
                if (state->get_value_from_vector(j+1)->get_double_from_map("count") >
                state->get_value_from_vector(j)->get_double_from_map("count"))
                {
                    state->swap_vector_components(j, j+1);
                    swapped = true;
                }
            }
        }

        // Pruning the least relevant items (keeping a large margin)
        while (state->get_vector_size() > n_top_items_)
        {
            state->pop_back_from_vector();
        }

        Value new_key;
        new_key.set_as_map();
        new_key.add_value_to_map("app")->set_string(out_app_name().c_str());
        new_key.add_value_to_map("concept")->set_string(key->get_string_from_map("concept"));
        emit_output(&new_key, state, writer);

        emit_state(key, state, writer);

        return true;
    }
    else
    {
        return false;
    }
}



}} // End of namespace
