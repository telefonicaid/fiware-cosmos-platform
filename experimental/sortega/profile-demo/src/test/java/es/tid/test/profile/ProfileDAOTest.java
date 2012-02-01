package es.tid.test.profile;

import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.Mongo;
import org.junit.*;
import static org.mockito.Mockito.*;
import static org.junit.Assert.*;

/**
 *
 * @author sebastian
 */
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
