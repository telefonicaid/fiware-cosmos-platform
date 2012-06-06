package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public class ItinRangeUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ItinRange> wrapper = ItinRangeUtil.createAndWrap(1, 2,
                3, 4, 5);
        wrapper.setConverter(ItinRange.class);
        ItinRange obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getPoiSrc());
        assertEquals(2, obj.getPoiTgt());
        assertEquals(3, obj.getNode());
        assertEquals(4, obj.getGroup());
        assertEquals(5, obj.getRange());
    }
}
