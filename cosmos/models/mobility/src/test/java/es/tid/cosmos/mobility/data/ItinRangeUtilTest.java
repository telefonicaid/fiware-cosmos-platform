package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class ItinRangeUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ItinRangeUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ItinRange> wrapper = ItinRangeUtil.createAndWrap(
                1L, 2L, 3, 4, 5);
        wrapper.setConverter(ItinRange.class);
        ItinRange obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getPoiSrc());
        assertEquals(2L, obj.getPoiTgt());
        assertEquals(3, obj.getNode());
        assertEquals(4, obj.getGroup());
        assertEquals(5, obj.getRange());
    }
}
