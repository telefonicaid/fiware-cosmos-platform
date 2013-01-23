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

package es.tid.cosmos.profile.api;

import java.util.Map.Entry;
import java.util.*;

/**
 *
 * @author dmicol, sortega
 */
public class CategoryMap extends HashMap<String, Long> {
    private static final Comparator<Entry<String, Long>> ENTRY_COMPARATOR =
            new Comparator<Entry<String, Long>>() {
                @Override
                public int compare(Entry<String, Long> left,
                                   Entry<String, Long> right) {
                    return right.getValue().compareTo(left.getValue());
                }
            };

    public List<String> getTop(int n) {
        if (this.isEmpty()) {
            return Collections.emptyList();
        }
        PriorityQueue<Entry<String, Long>> heap =
                new PriorityQueue<Entry<String, Long>>(this.size(),
                                                       ENTRY_COMPARATOR);
        heap.addAll(this.entrySet());
        List<String> topN = new ArrayList<String>(n);
        for (int i = 0; i < n && !heap.isEmpty(); i++) {
            topN.add(heap.remove().getKey());
        }
        return topN;
    }

    public List<String> getAll() {
        return this.getTop(this.size());
    }
}