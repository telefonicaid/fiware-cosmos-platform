#ifndef _H_SAMSON_KVRANGE
#define _H_SAMSON_KVRANGE

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "logMsg/logMsg.h"

#include "samson/common/common.h"
#include "samson/common/samson.pb.h"

namespace samson {
struct KVInfo;
struct FullKVInfo;

/**
 * KVRange keeps information about a particular range of hash-groups
 */

struct KVRange {
    int hg_begin;
    int hg_end;

    KVRange();
    KVRange(int _hg_begin, int _hg_end);

    // Tried to set explicit, but an assignment is needed between gpb:KVRange and samson:KVRange
    // TODO(@andreu): please, check the class conversion
    KVRange(const gpb::KVRange& range) {
      hg_begin = range.hg_begin();
      hg_end = range.hg_end();
    }

    KVRange operator=(const gpb::KVRange& range) {
      hg_begin = range.hg_begin();
      hg_end = range.hg_end();
      return *this;
    }

    void fill(gpb::KVRange *range) {
      range->set_hg_begin(hg_begin);
      range->set_hg_end(hg_end);
    }

    void set(int _hg_begin, int _hg_end);
    void setFrom(KVInfo *info);
    void setFrom(FullKVInfo *info);
    void add(KVRange range);

    bool isValid() const;
    std::string str() const;
    void getInfo(std::ostringstream& output) const;
    int getNumHashGroups() const;
    int size() const;

    bool IsOverlapped(const KVRange& range) const;
    bool includes(KVRange range) const;
    bool contains(int hg) const;
    bool contains(KVRange range) const;

    // Get the maximum division pattern for this range
    bool isValidForNumDivisions(int num_divisions) const;
    ;
    int getMaxNumDivisions() const;
    ;

    // Check if this range is valid for this KVInfo*
    bool check(KVInfo *info) const;

    // Compute the intersection between two ranges

    KVRange Intersection(KVRange range) {
      // Disjoint ranges
      if (range.hg_end <= hg_begin) {
        return KVRange(0, 0);
      }

      if (range.hg_begin >= hg_end) {
        return KVRange(0, 0);
      }

      return KVRange(std::max(hg_begin, range.hg_begin), std::min(hg_end, range.hg_end));
    }

    std::vector<KVRange> divide(int factor) const {
      // Max factor
      if (factor > (hg_end - hg_begin))
        factor = hg_end - hg_begin;

      // Divide range in a number of sub ranges
      int length = (hg_end - hg_begin) / factor;

      std::vector<KVRange> ranges;
      for (int i = 0; i < (factor - 1); i++)
        ranges.push_back(KVRange(hg_begin + i * length, hg_begin + (i + 1) * length));
      ranges.push_back(KVRange(hg_begin + (factor - 1) * length, hg_end));
      return ranges;
    }
};

// Operations to compare ranges
bool operator<(const KVRange & left, const KVRange & right);
bool operator==(const KVRange & left, const KVRange & right);
bool operator!=(const KVRange & left, const KVRange & right);

KVRange rangeForDivision(int pos, int num_divisions);
int divisionForHashGroup(int hg, int num_division);

KVRange rangeForIntersection(const KVRange & left, const KVRange & right);

/* ------------------------------------------------------------------
 *
 * class KVRanges
 *
 * Collection of ranges ( KVRange )
 *
 * ------------------------------------------------------------------*/

class KVRanges {
  public:
    KVRanges() {
    }

    KVRanges(const gpb::KVRanges& ranges) {
      for (int i = 0; i < ranges.range_size(); i++) {
        ranges_.push_back(ranges.range(i));
      }
    }

    bool IsFullRange() {
      for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
        if (!Contains(i)) {
          return false;
        }
      }
      return true;
    }

    int RandomHashGroup() {
      int pos = random() % size();

      for (size_t i = 0; i < ranges_.size(); i++) {
        if (pos < ranges_[i].size()) {
          return ranges_[i].hg_begin + pos;
        } else {
          pos -= ranges_[i].size();
        }
      }
      LM_E(("Internal error"));
      return -1;
    }

    void fill(gpb::KVRanges *ranges) {
      for (size_t i = 0; i < ranges_.size(); i++) {
        ranges_[i].fill(ranges->add_range());
      }
    }

    // Implicit conversion to a vector
    operator std::vector<KVRange>() const {
      return ranges_;
    }

    // Function to remove a range from a range ( it is suppoused to be an intersection )
    int SetFromDifference(const KVRange& target, const KVRange& range) {
      if (range.hg_begin < target.hg_begin) {
        return 1;   // Error ( we only remove intersections )
      }
      if (range.hg_end > target.hg_end) {
        return 1;   // Error ( we only remove intersections )
      }
      // First part
      if (range.hg_begin > target.hg_begin) {
        Add(KVRange(target.hg_begin, range.hg_begin));
      }

      // Last part
      if (range.hg_end < target.hg_end) {
        Add(KVRange(range.hg_end, target.hg_end));
      }

      return 0;
    }

    void Add(const KVRange& hg_range) {
      ranges_.push_back(hg_range);
    }

    void Add(const KVRanges& ranges) {
      for (size_t i = 0; i < ranges.ranges_.size(); i++) {
        Add(ranges.ranges_[i]);
      }
    }

    int Remove(const KVRange& range) {
      if (range.hg_begin == range.hg_end) {
        return 0;   // No real range to be removed
      }
      // Loop internal ranges to
      for (size_t i = 0; i < ranges_.size(); i++) {
        // Compute intersection
        KVRange intersection = ranges_[i].Intersection(range);

        // If no intersection, continue to the next range
        if (intersection.size() == 0) {
          continue;
        }

        // Remove intersection to both
        KVRanges remain_ranges;
        KVRanges remain_ranges_to_remove;
        int rc = remain_ranges.SetFromDifference(ranges_[i], intersection);
        if (rc) {
          return rc;
        }

        int rc2 = remain_ranges_to_remove.SetFromDifference(range, intersection);
        if (rc2) {
          return rc2;
        }

        // Add remainings
        ranges_.erase(ranges_.begin() + i);
        Add(remain_ranges);

        // Remove the rest of the range ( if any )
        for (size_t j = 0; j < remain_ranges_to_remove.ranges_.size(); j++) {
          int rc = Remove(remain_ranges_to_remove.ranges_[j]);
          if (rc) {
            return rc;
          }
        }

        // Everything removed correctly
        return 0;
      }

      return 1;   // It is not possible to remove this range
    }

    std::string str() {
      std::ostringstream output;

      output << "{ ";
      for (size_t i = 0; i < ranges_.size(); i++) {
        output << ranges_[i].str() << " ";
      }
      output << "}";
      return output.str();
    }

    // Check if it overladps with this range
    bool IsOverlapped(const KVRange& range) const {
      for (size_t i = 0; i < ranges_.size(); i++) {
        if (ranges_[i].IsOverlapped(range)) {
          return true;
        }
      }
      return false;
    }

    bool IsOverlapped(const KVRanges& ranges) const {
      for (size_t i = 0; i < ranges.ranges_.size(); i++) {
        if (IsOverlapped(ranges.ranges_[i])) {
          return true;
        }
      }
      return false;
    }

    bool Contains(int hg) const {
      for (size_t i = 0; i < ranges_.size(); i++) {
        if (ranges_[i].contains(hg)) {
          return true;
        }
      }
      return false;
    }

    double GetOverlapFactor(const KVRange& range) {
      size_t range_in = 0;
      size_t range_total = 0;

      for (size_t i = 0; i < ranges_.size(); i++) {
        KVRange intersection = ranges_[i].Intersection(range);

        range_total += ranges_[i].size();
        range_in += intersection.size();
      }

      if (range_total == 0) {
        // TODO(@jges): Remove log message
        LM_M(("overlap_factor(0) because range_total(%lu)", range_total));

        return 0;
      }

      // TODO(@jges): Remove log message
      LM_M(("overlap_factor(%lf) = range_in(%lu)/range_total(%lu)", (static_cast<double>(range_in) / static_cast<double>(range_total)), range_in, range_total));
      return (static_cast<double>(range_in) / static_cast<double>(range_total));
    }

    int size() {
      int total_size = 0;

      for (size_t i = 0; i < ranges_.size(); i++) {
        total_size += ranges_[i].size();
      }
      return total_size;
    }

    KVRange max_range() const {
      if (ranges_.size() == 0)
        return KVRange(0, 0);
      KVRange range = ranges_[0];
      for (size_t i = 1; i < ranges_.size(); i++) {
        if (ranges_[i].hg_begin < range.hg_begin)
          range.hg_begin = ranges_[i].hg_begin;
        if (ranges_[i].hg_end > range.hg_end)
          range.hg_end = ranges_[i].hg_end;
      }
      return range;
    }

  private:
    std::vector<KVRange> ranges_;
};
}

#endif  // ifndef _H_SAMSON_KVRANGE
