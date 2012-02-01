package es.tid.test.profile;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;

/**
 *
 * @author sortega
 */
public class ProfileDAO {

    private static final DBObject CATEGORIES_COL =
                                  new BasicDBObject("categories", true);
    private static final DBObject DATE_COL =
                                  new BasicDBObject("date", -1);
    private final DBCollection profiles;

    public ProfileDAO(DBCollection profiles) {
        this.profiles = profiles;
    }

    /**
     * Get the latest category map for a given user.
     * 
     * @param userId  Unique user id.
     * @return        CategoryMap or null.
     */
    public CategoryMap getLastCategoryMap(String userId) {

        BasicDBObject ref = new BasicDBObject("id", userId);
        for (DBObject result : this.profiles.find(ref, CATEGORIES_COL).sort(
                DATE_COL).limit(1)) {

            CategoryMap categoryMap = new CategoryMap();
            return toCategoryMap((DBObject) result.get("categories"), categoryMap);
        }
        
        return new CategoryMap();
    }

    private CategoryMap toCategoryMap(DBObject resultCategories,
                                      CategoryMap categoryMap) {
        for (String categoryName : resultCategories.keySet()) {
            categoryMap.put(categoryName, (Double) resultCategories.get(
                    categoryName));
        }
        return categoryMap;
    }
}
