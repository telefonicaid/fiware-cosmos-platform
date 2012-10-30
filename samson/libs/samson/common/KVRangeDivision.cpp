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
#include "KVRangeDivision.h" // Own interface


namespace samson {

  KVRangeDivision::KVRangeDivision() {
    AddDivision(0);
    AddDivision(KVFILE_NUM_HASHGROUPS);
  }
  
  KVRangeDivision::KVRangeDivision(int num_divisions) {
    for (int i = 0; i < num_divisions; ++i) {
      AddDivision(GetKVRangeForDivision(i, num_divisions));
    }
  }
  
  void KVRangeDivision::AddDivision(const KVRange& range) {
    AddDivision(range.hg_begin_);
    AddDivision(range.hg_end_);
  }
  
  void KVRangeDivision::AddDivision(const std::vector<KVRange>& ranges) {
    size_t ranges_size = ranges.size();
    for (size_t i = 0; i < ranges_size; ++i) {
      AddDivision(ranges[i]);
    }
  }
  
  void KVRangeDivision::AddDivision(int hg) {
    if (hg < 0) {
      return;
    }
    if (hg > KVFILE_NUM_HASHGROUPS) {
      return;
    }
    divisions_.insert(hg);
  }
  
  std::vector<KVRange> KVRangeDivision::ranges() const {
    if (divisions_.size() < 2) {
      LM_X(1, ("Internal error"));
    }
    
    std::set<int>::iterator it;
    std::vector<int> divisions;
    for (it = divisions_.begin(); it != divisions_.end(); ++it) {
      divisions.push_back(*it);
    }
    
    // Create output vector of ranges
    std::vector<KVRange> ranges;
    size_t divisions_size_but_1 = divisions.size() - 1;
    for (size_t i = 0; i < divisions_size_but_1; ++i) {
      ranges.push_back(KVRange(divisions[i], divisions[i + 1]));
    }
    return ranges;
  }

}