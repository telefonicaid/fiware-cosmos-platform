package es.tid.cosmos.profile.api;

import java.util.List;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
import javax.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

/**
 *
 * @author sortega
 */
@Component
public class ProfileDAO {
    private static final String PROFILE_COLLECTION = "profile";
    private static final DBObject CATEGORIES_COL =
            new BasicDBObject("categories", true);
    private static final DBObject DATE_COL =
            new BasicDBObject("date", -1);
    @Autowired(required = true)
    private MongoService mongo;
    private DBCollection profiles;

    @PostConstruct
    public void init() {
        this.profiles = this.mongo.getDb().getCollection(PROFILE_COLLECTION);
    }

    public void setMongo(MongoService mongo) {
        this.mongo = mongo;
    }

    /**
     * Get the latest category map for a given user.
     *
     * @param userId Unique user id.
     *
     * @return CategoryMap or null.
     */
    public CategoryMap getLastCategoryMap(String userId) {
        for (DBObject result : this.getCategories(userId)) {
            CategoryMap categoryMap = new CategoryMap();
            return toCategoryMap((DBObject) result.get("categories"),
                                 categoryMap);
        }
        return new CategoryMap();
    }

    public List<DBObject> getCategories(String userId) {
        BasicDBObject ref = new BasicDBObject("id", userId);
        return (this.profiles
                .find(ref, CATEGORIES_COL)
                .sort(DATE_COL)
                .limit(1)
                .toArray());
    }

    private CategoryMap toCategoryMap(DBObject resultCategories,
                                      CategoryMap categoryMap) {
        for (String categoryName : resultCategories.keySet()) {
            Number count = (Number) resultCategories.get(categoryName);
            categoryMap.put(categoryName, count.longValue());
        }
        return categoryMap;
    }
}
