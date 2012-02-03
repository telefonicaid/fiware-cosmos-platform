package es.tid.test.profile;

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
    private ProfileDAO instance;

    @Before
    public void setUp() throws Exception {
        MongoService mongo = new MongoService("localhost", "individualprofile");
        this.instance = new ProfileDAO();
        this.instance.setMongo(mongo);
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
