package es.tid.cosmos.profile.userprofile;

import java.util.*;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;

/**
 * Aggregates and sort CategoryCount objects by decresing count order.
 *
 * @author sortega
 */
class CategoryCountAggregator {
    private Map<String, CategoryCount.Builder> partials;

    public CategoryCountAggregator() {
        this.partials = new HashMap<String, CategoryCount.Builder>();
    }

    public void clear() {
        this.partials.clear();
    }

    public void add(CategoryCount count) {
        CategoryCount.Builder partial = this.partials.get(count.getName());
        if (partial != null) {
            partial.setCount(partial.getCount() + count.getCount());
        } else {
            this.partials.put(count.getName(), count.toBuilder());
        }
    }

    /**
     * Returns accumulated CategoryCount list in decreasing count order.
     *
     * @return
     */
    public List<CategoryCount> getSortedCounts() {
        LinkedList<CategoryCount> counts = new LinkedList<CategoryCount>();
        for (CategoryCount.Builder partial : this.partials.values()) {
            counts.add(partial.build());
        }
        Collections.sort(counts, new Comparator<CategoryCount>() {
            @Override
            public int compare(CategoryCount left, CategoryCount right) {
                if (left.getCount() > right.getCount()) {
                    return -1;
                } else if (left.getCount() < right.getCount()) {
                    return 1;
                } else {
                    return 0;
                }
            }
        });
        return counts;
    }
}
