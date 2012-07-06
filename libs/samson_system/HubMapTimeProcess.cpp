
#include "samson_system/ValueContainer.h"

#include "HubMapProcess.h" // Own interface


namespace samson{ namespace system{

bool HubMapProcess::update( Value* key ,Value * state ,Value ** values ,size_t num_values ,samson::KVWriter *writer )
{
    Value new_key;
    Value new_value;

    if (key->checkMapValue("app", name().c_str()))
    {
        std::vector<std::string> keys = key->get_keys_from_map();
        for (size_t i = 0; (i < keys.size()); i++)
        {
            if ((keys[i] == "app") || (keys[i] == "timestamp"))
            {
                continue;
            }
            new_key.set_as_map();
            new_key.add_value_to_map("app")->set_string(out_app_name().c_str());
            new_key.add_value_to_map(keys[i])->set_string("concept");
            new_value.add_value_to_map("item")->set_string( key->get_string_from_map(keys[i].c_str()));
            new_value.add_value_to_map("time")->set_double( key->get_double_from_map("timestamp"));
            for (size_t j = 0; (j < num_values); j++)
            {
                emit_output(&new_key, &new_value, writer);
            }
        }
        key->set_string_for_map("app", out_def_name().c_str());
        for (size_t j = 0; (j < num_values); j++)
        {
            emit_feedback(key, values[j], writer);
        }
        return false;
    }
    else
    {
        return false;
    }
}



}} // End of namespace
