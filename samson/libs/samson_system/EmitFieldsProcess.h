#ifndef SAMSON_LIBS_SAMSON_SYSTEM_EMITFIELDSPROCESS_H_
#define SAMSON_LIBS_SAMSON_SYSTEM_EMITFIELDSPROCESS_H_
/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) 2012 Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * FILE            EmitFieldsProcess.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Declaration of EmitFieldsProcess class to select and reemit fields
 *  from a system.Value key, under the Process paradigm
 *
 */
#include <string>
#include <vector>

#include "samson_system/ProcessComponent.h"
#include "samson_system/Value.h"
#include "samson/module/KVWriter.h"

/**
 * \file EmitFieldsProcess.h
 * \mainpage EmitFieldsProcess
 * \author Gregorio Escalada
 * \date August 2012
 *
 * Component for system.Value paradigm, for data flexible processing
 *
 */

namespace samson {
namespace system {
/**
 * \class EmitFieldsProcess
 * \brief Class to emit specific fields from input key-values.
 * Implements the Update() method for the system.Value process paradigm.
 * Input data is identified with name_ as "app" field.
 * Fields to be emitted are selected by name with the vector concepts_
 * It is possible to associated them with an independent concept
 * New values are written to the feedback queue with out_app_name_.
 * Original data is written to the feedback queue with out_def_name_.
 * New values can be created with an additional field (timestamp typically).
 * This ProcessComponent just reemit data to the feedback queue, there is no output nor state associated
 */
class EmitFieldsProcess : public ProcessComponent {
  public:
    static const std::string kNullField;
    static const std::string kNullDest;

    EmitFieldsProcess()
    : ProcessComponent("default")
    , out_app_name_("def_out")
    , out_def_name_("def_def")
    , independent_concept_(kNullField)
    , include_independent_(false)
    , additional_field_(kNullField)
    , include_field_(false) {
      concepts_.clear();
    }

    EmitFieldsProcess(std::string name = "default", const std::string& out_app = "def_out",
        const std::string& out_def = "def_def", std::vector<std::string> concepts = std::vector<std::string>(),
        const std::string& independent_concept = kNullField, const std::string& additional_field = kNullField)
    : ProcessComponent(name)
    , out_app_name_(out_app)
    , out_def_name_(out_def)
    , concepts_(concepts)
    , independent_concept_(independent_concept)
    , additional_field_(additional_field) {
      if (independent_concept_ != kNullField) {
        include_independent_ = true;
      }
      if (additional_field_ != kNullField) {
        include_field_ = true;
      }
    }

    ~EmitFieldsProcess() {
      concepts_.clear();
    }

    std::string out_app_name() {
      return out_app_name_;
    }

    std::string out_def_name() {
      return out_def_name_;
    }

    // Update this state based on input values ( return true if this state has been updated with this component )
    bool Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);

  private:
    std::string out_app_name_;
    std::string out_def_name_;
    std::vector<std::string> concepts_;
    std::string independent_concept_;
    bool include_independent_;
    std::string additional_field_;
    bool include_field_;
};
}
}   // End of namespace

#endif   // ifndef SAMSON_LIBS_SAMSON_SYSTEM_EMITFIELDSPROCESS_H_
