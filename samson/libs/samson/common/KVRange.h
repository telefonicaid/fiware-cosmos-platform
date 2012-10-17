#ifndef _H_SAMSON_KVRANGE
#define _H_SAMSON_KVRANGE

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <set>

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

  void RemoveIntersection( const KVRange& range )
  {
    // Remove intersection with this range
    
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

  double GetOverlapFactor (const KVRange& range) const
  {
    return (double) Intersection(range).size() / (double)size();
  }
    bool IsOverlapped(const KVRange& range) const;
  bool IsOverlapped(const std::vector<KVRange>& ranges)
  {
    for ( size_t i = 0 ; i < ranges.size() ; i++ )
      if( IsOverlapped(ranges[i]))
        return true;
    return false;
  }
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

    KVRange Intersection(KVRange range) const {
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

  // Complete set of ranges to cover the entire map of hgs
  class KVRangeDivision
  {

    std::set<int> divisions_; // Pointer where a new KVRange will be defined

  public:

    KVRangeDivision( )
    {
      AddDivision(0);
      AddDivision(KVFILE_NUM_HASHGROUPS);
    }

    KVRangeDivision( int num_divisions )
    {
      for ( int i = 0 ; i < num_divisions ; i++ )
        Force( rangeForDivision(i,num_divisions) );
    }

    void Force( const KVRange& range )
    {
      AddDivision( range.hg_begin );
      AddDivision( range.hg_end );
    }

    void Force( const std::vector<KVRange>& ranges )
    {
      for ( size_t i = 0 ; i < ranges.size() ; i++ )
        Force( ranges[i]);
    }

    void AddDivision( int hg )
    {
      if( hg < 0 )
        return;
      if( hg > KVFILE_NUM_HASHGROUPS )
        return;
      divisions_.insert(hg);
    }

    std::vector<KVRange> ranges()
    {

      if( divisions_.size() < 2 )
      {
        LM_X(1, ("Internal error"));
      }

      std::set<int>::iterator it;
      std::vector<int> divisions;
      for ( it = divisions_.begin() ; it != divisions_.end() ; it++ ){
        divisions.push_back(*it);
    }

      // Create output vector of ranges
      std::vector<KVRange> ranges;
      for ( size_t i = 0 ; i < divisions.size()-1 ; i++ )
        ranges.push_back( KVRange(  divisions[i], divisions[i+1] ) );

      return ranges;

      }

    static KVRange rangeForDivision(int pos, int num_divisions) {
      int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;
      int hg_begin = pos * size_per_division;
      int hg_end = (pos + 1) * size_per_division;

      if (pos == (num_divisions - 1)) {
        hg_end = KVFILE_NUM_HASHGROUPS;
        }

      return KVRange(hg_begin, hg_end);
        }

  };

// Operations to compare ranges
bool operator<(const KVRange & left, const KVRange & right);
bool operator==(const KVRange & left, const KVRange & right);
bool operator!=(const KVRange & left, const KVRange & right);

KVRange rangeForDivision(int pos, int num_divisions);
int divisionForHashGroup(int hg, int num_division);

KVRange rangeForIntersection(const KVRange & left, const KVRange & right);

  std::string str( const std::vector<KVRange>& ranges );


  bool CheckCompleteKVRanges( const std::vector<KVRange>& ranges );

}

#endif  // ifndef _H_SAMSON_KVRANGE
