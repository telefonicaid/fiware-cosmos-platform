package es.tid.cosmos.profile.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryCount;

/**
 * Test case for CategoryCountUtil
 *
 * @author dmicol
 */
public class CategoryCountUtilTest {
    @Test
    public void shouldBeUtilityClass() {
        assertUtilityClass(CategoryCountUtil.class);
    }

    @Test
    public void testCreate() {
        final String name = "Sports";
        final long count = 5L;

        ProtobufWritable<CategoryCount> catCountWrapper = CategoryCountUtil.
                createAndWrap(name, count);
        CategoryCount catCount = catCountWrapper.get();
        assertEquals(name, catCount.getName());
        assertEquals(count, catCount.getCount());
    }
}
