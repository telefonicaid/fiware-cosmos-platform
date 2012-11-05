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
#ifndef _H_SAMSON_WORKER_SAMPLES
#define _H_SAMSON_WORKER_SAMPLES

#include <iomanip>                              // setiosflags()
#include <iostream>                             // std::cout

#include "logMsg/logMsg.h"                      //

#include "au/console/Console.h"
#include "au/network/RESTService.h"

#include "engine/EngineElement.h"               // samson::EngineElement

#include "samson/stream/WorkerTaskManager.h"     // samson::stream::WorkerTaskManager

#include "samson/common/Macros.h"               // exit(.)
#include "samson/common/NotificationMessages.h"
#include "samson/common/samson.pb.h"            // samson::network::
#include "samson/common/samsonDirectories.h"    // SAMSON_WORKER_DEFAULT_PORT

#include "samson/worker/SamsonWorkerController.h"

#include "samson/delilah/Delilah.h"
#include "samson/network/NetworkInterface.h"
#include "samson/worker/WorkerCommandManager.h"


namespace samson {
class NetworkInterface;
class Info;

class SampleVector {
  // Vector of numbers
  double *values_;
  int max_num_values_;

public:

  SampleVector(int max_num_values) {
    max_num_values_ = max_num_values;
    values_ = new double[max_num_values_];
  }

  ~SampleVector() {
    delete[] values_;
  }

  void push_to_sample_vector(double value) {
    for (int i = 1; i < max_num_values_; i++) {
      values_[ i - 1 ] = values_[ i ];
    }
    values_[ max_num_values_ - 1 ] = value;
  }

  int getVector(double *v, int max_v_length) {
    if (max_v_length < max_num_values_) {
      return 0;           // Not possible to fit here
    }
    for (int i = 0; i < max_num_values_; i++) {
      v[i] = values_[i];
    }
    return max_num_values_;
  }

  int get_num_values() {
    return max_num_values_;
  }
};

class AccumulatedSampleVector : public SampleVector {
  // Number of samples to average to generate a new one
  int num_averge_samples_;

  // Accumulated number
  double accumulated_value_;         // Acumulated sum
  int num_values_;                   // Number of accumulated values

public:

  AccumulatedSampleVector(int num_averge_samples, int num_values) : SampleVector(num_values) {
    num_averge_samples_ = num_averge_samples;

    // Accumulated version
    accumulated_value_ = 0;
    num_values_ = 0;
  }

  void push(double value) {
    accumulated_value_ += value;
    num_values_++;

    if (num_values_ >= num_averge_samples_) {
      push_to_sample_vector(accumulated_value_ / (double)num_values_);

      // Init again
      accumulated_value_ = 0;
      num_values_ = 0;
    }
  }
};


class FullSampleVector {
  AccumulatedSampleVector second;
  AccumulatedSampleVector minute;
  // AccumulatedSampleVector hour;
  // AccumulatedSampleVector day;

public:

  FullSampleVector() : second(1, 60), minute(60, 60) {         // , hour( 60*60 , 24 ) , day( 60*60*24 , 30 )
  }

  void push(double value) {
    second.push(value);
    minute.push(value);
    // hour.push( value );
    // day.push( value );
  }

  int get_num_values() {
    int total = 0;

    total += second.get_num_values();
    total += minute.get_num_values();
    // total += hour.get_num_values();
    // total += day.get_num_values();

    return total;
  }

  // Get the vector of values
  int getVector(double *v, int max_v_length) {
    if (max_v_length < get_num_values()) {
      LM_W(("Not enougth length for FullSampleVector::getVector"));
      return 0;
    }

    int pos = 0;
    // pos += day.getVector( &v[pos] , max_v_length - pos );
    // pos += hour.getVector( &v[pos] , max_v_length - pos );
    pos += minute.getVector(&v[pos], max_v_length - pos);
    pos += second.getVector(&v[pos], max_v_length - pos);
    return pos;
  }

  std::string getJson(std::string label) {
    /*
     * {
     * "label": "CPU",
     * "data": [[-100, values], [-99, value]... ]
     * }
     */

    int num_values = get_num_values();
    double values[ num_values  ];
    int n = getVector(values, num_values);


    std::ostringstream output;

    output << "{";
    output << "\"label\":\"" << label << "\",";

    output << "\"data\":";
    output << "[";
    for (int i = 0; i < n; i++) {
      output << "[" <<
      (i - n + 1) << "," << std::setiosflags(std::ios::fixed) << std::setprecision(2) << values[i] << "]";
      if (i != (n - 1)) {
        output << ",";
      }
    }
    output << "]";
    output << "}";

    return output.str();
  }
};


class SamsonWorker;
class SamsonWorkerSamples {
  SamsonWorker *samsonWorker_;

  FullSampleVector cpu;
  FullSampleVector memory;
  FullSampleVector disk_in;
  FullSampleVector disk_out;
  FullSampleVector net_in;
  FullSampleVector net_out;

public:

  SamsonWorkerSamples(SamsonWorker *samsonWorker) {
    samsonWorker_ = samsonWorker;
  }

  // Take necessary samples for cpu, memory,...
  void take_samples();

  std::string getJson() {
    std::ostringstream output;

    output << "[";
    output << cpu.getJson("cpu");
    output << ",";
    output << memory.getJson("memory");
    output << ",";
    output << disk_in.getJson("disk in");
    output << ",";
    output << disk_out.getJson("disk out");
    output << ",";
    output << net_in.getJson("net in");
    output << ",";
    output << net_out.getJson("net out");
    output << "]";
    return output.str();
  }
};
}

#endif  // ifndef _H_SAMSON_WORKER_SAMPLES

