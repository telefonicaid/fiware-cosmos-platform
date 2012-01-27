package es.tid.test.hbase;

import java.util.Map.Entry;
import java.util.*;

/**
 *
 * @author sortega
 */
public class CategoryMap extends HashMap<String, Double> {

    private static final Comparator<Entry<String, Double>> ENTRY_COMPARATOR =
            new Comparator<Entry<String, Double>>() {
                @Override
                public int compare(Entry<String, Double> left, 
                                   Entry<String, Double> right) {
                    return left.getValue().compareTo(right.getValue());
                }
            };

    public List<String> getTop(int n) {
        PriorityQueue<Entry<String, Double>> heap =
                new PriorityQueue<Entry<String, Double>>(this.size(), 
                ENTRY_COMPARATOR);

        heap.addAll(this.entrySet());
        
        List<String> topN = new ArrayList<String>(n);
        for (int i = 0; i < n && !topN.isEmpty(); i++) {
            topN.add(heap.remove().getKey());
        }
        return topN;
    }
}
