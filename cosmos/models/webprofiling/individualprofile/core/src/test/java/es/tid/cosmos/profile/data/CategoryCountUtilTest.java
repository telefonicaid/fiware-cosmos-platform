package es.tid.cosmos.profile.data;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;

/**
 * Test case for CategoryCountUtil
 *
 * @author dmicol
 */
public class CategoryCountUtilTest {
    private CategoryCountUtil instance;

    @Before
    public void setUp() {
        this.instance = new CategoryCountUtil();
    }

    @Test
    public void testCreate() {
        CategoryCount count = CategoryCountUtil.create("Sports", 5L);
        assertEquals("Sports", count.getName());
        assertEquals(5L, count.getCount());
    }
}
