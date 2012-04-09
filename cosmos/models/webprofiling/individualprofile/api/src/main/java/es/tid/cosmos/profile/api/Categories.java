package es.tid.bdp.profile.api;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author dmicol, sortega
 */
public final class Categories {
    private Categories() {
    }
    
    public static List getAll(ProfileDAO profile, String userName) {
        return getTop(profile, userName, null);
    }
    
    public static List getTop(ProfileDAO profile, String userName, Integer n) {
        if (n != null && n < 1) {
            throw new IllegalArgumentException("Invalid number of elements");
        }

        final CategoryMap categoryMap = profile.getLastCategoryMap(userName);
        List topN = new LinkedList();
        List<String> categories = (n == null ? categoryMap.getAll()
                                             : categoryMap.getTop(n));
        for (final String category : categories) {
            topN.add(new HashMap() {{
                put("category", category);
                put("count", categoryMap.get(category));
            }});
        }
        return topN;
    }
}
