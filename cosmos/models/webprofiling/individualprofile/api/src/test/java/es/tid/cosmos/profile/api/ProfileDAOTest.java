package es.tid.cosmos.profile.api;

import java.util.ArrayList;
import java.util.List;

import com.mongodb.BasicDBObject;
import com.mongodb.DBObject;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol, sortega
 */
public class ProfileDAOTest {
    private ProfileDAO instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new ProfileDAO() {
            @Override
            public List<DBObject> getCategories(String userId) {
                List<DBObject> categories = new ArrayList<DBObject>();
                if ("X1234".equals(userId)) {
                    categories.add(new BasicDBObject("categories",
                            new BasicDBObject("Social networks", 2L)));
                    categories.add(new BasicDBObject("categories",
                            new BasicDBObject("Enterntainment", 9L)));
                } else {
                    // Don't add any categories for other users
                }
                return categories;
            }
        };
    }

    @Test
    public void shouldRetrieveLastCategoryMap() {
        CategoryMap categories = this.instance.getLastCategoryMap("X1234");
        assertEquals("Social networks", categories.getTop(1).get(0));
    }

    @Test
    public void shouldGiveEmptyIfNotFound() {
        assertTrue(this.instance.getLastCategoryMap("NonExistingUser").isEmpty());
    }
}
