package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class PoiUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(PoiUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Poi> wrapper = PoiUtil.createAndWrap(1, 2, 3, 4, 5, 6,
                7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
        Poi obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getId());
        assertEquals(2, obj.getNode());
        assertEquals(3, obj.getBts());
        assertEquals(4, obj.getLabelnode());
        assertEquals(5, obj.getLabelgroupnode());
        assertEquals(6, obj.getConfidentnode());
        assertEquals(7D, obj.getDistancenode(), 0.0D);
        assertEquals(8, obj.getLabelbts());
        assertEquals(9, obj.getLabelgroupbts());
        assertEquals(10, obj.getConfidentbts());
        assertEquals(11D, obj.getDistancebts(), 0.0D);
        assertEquals(12, obj.getLabelnodebts());
        assertEquals(13, obj.getLabelgroupnodebts());
        assertEquals(14, obj.getConfidentnodebts());
        assertEquals(15D, obj.getDistancenodebts(), 0.0D);
        assertEquals(16, obj.getInoutWeek());
        assertEquals(17, obj.getInoutWend());
    }
}
