package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;

/**
 *
 * @author dmicol
 */
public class PoiNewUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<PoiNew> wrapper = PoiNewUtil.createAndWrap(1, 2, 3,
                                                                    4, 5);
        PoiNew obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getId());
        assertEquals(2, obj.getNode());
        assertEquals(3, obj.getBts());
        assertEquals(4, obj.getLabelgroupnodebts());
        assertEquals(5, obj.getConfidentnodebts());
    }
}
