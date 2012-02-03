package es.tid.test.profile;

import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.Mongo;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

/**
 *
 * @author sortega
 */
@Ignore
public class ProfileDAOTest {
    private DBCollection profileCollection;
    private ProfileDAO instance;

    @Before
    public void setUp() throws Exception {
        //profileCollection = mock(DBCollection.class);
        Mongo mongo = new Mongo();
        DB db = mongo.getDB("individualprofile");
        this.profileCollection = db.getCollection("profile");
        this.instance = new ProfileDAO(profileCollection);
    }

    @Test
    public void shouldRetrieveLastCategoryMap() {
        CategoryMap categories = instance.getLastCategoryMap("X1234");
        assertEquals("Social networks", categories.getTop(1).get(0));
    }

    @Test
    public void shouldGiveEmptyIfNotFound() {
        assertTrue(instance.getLastCategoryMap("NonExistingUser").isEmpty());
    }
}
