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
#ifndef _H_SAMSON_KVRANGE
#define _H_SAMSON_KVRANGE

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "logMsg/logMsg.h"

#include "samson/common/common.h"


namespace samson {
struct KVInfo;
struct FullKVInfo;

/**
 * KVRange keeps information about a particular range of hash-groups
 */

struct KVRange {
  int hg_begin_;
  int hg_end_;

  KVRange() : hg_begin_(0), hg_end_(KVFILE_NUM_HASHGROUPS) {
  }

  KVRange(int hg_begin, int hg_end) : hg_begin_(hg_begin), hg_end_(hg_end) {
  }

  KVRange(const gpb::KVRange& range) {      // Automatic conversion from gpb::KVRange and KVRange is allowed
    hg_begin_ = range.hg_begin();
    hg_end_ = range.hg_end();
  }

  KVRange operator=(const gpb::KVRange& range) {
    hg_begin_ = range.hg_begin();
    hg_end_ = range.hg_end();
    return *this;
  }

  void fill(gpb::KVRange *range) const {
    range->set_hg_begin(hg_begin_);
    range->set_hg_end(hg_end_);
  }

  // Differnt methods to set the limits of this range
  void set(int _hg_begin, int _hg_end);
  void setFrom(KVInfo *info);
  void setFrom(FullKVInfo *info);
  void add(KVRange range);


  // Methods to check content
  bool isValid() const;
  int size() const;
  std::string str() const;
  double GetOverlapFactor(const KVRange& range) const;
  bool IsOverlapped(const KVRange& range) const;
  bool IsOverlapped(const std::vector<KVRange>& ranges);
  bool Includes(KVRange range) const;
  bool Contains(int hg) const;
  bool Contains(KVRange range) const;
  KVRange Intersection(KVRange range) const;
  std::vector<KVRange> divide(int factor) const;
};

// Operations to compare ranges
bool operator<(const KVRange & left, const KVRange & right);
bool operator==(const KVRange & left, const KVRange & right);
bool operator!=(const KVRange & left, const KVRange & right);

// Miscelanious opertions with KVRanges
KVRange GetKVRangeForDivision(int pos, int num_divisions);
int GetKVRangeDivisionForHashGroup(int hg, int num_division);
std::string str(const std::vector<KVRange>& ranges);
bool CheckCompleteKVRanges(const std::vector<KVRange>& ranges);
}

#endif  // ifndef _H_SAMSON_KVRANGE
