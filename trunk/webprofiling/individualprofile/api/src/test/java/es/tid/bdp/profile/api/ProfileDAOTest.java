package es.tid.bdp.profile.api;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.mockito.Mockito;

/**
 *
 * @author sortega
 */
@Ignore
public class ProfileDAOTest {
    private ProfileDAO instance;

    @Before
    public void setUp() throws Exception {
        MongoService mongo = Mockito.mock(MongoService.class);
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
