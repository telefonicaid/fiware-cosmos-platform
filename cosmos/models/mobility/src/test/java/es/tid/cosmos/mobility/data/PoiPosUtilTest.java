package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class PoiPosUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(PoiPosUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<PoiPos> wrapper = PoiPosUtil.createAndWrap(1, 2, 3, 4,
                5, 6, 7, 8, 9, 10, 11);
        PoiPos obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getNode());
        assertEquals(2, obj.getBts());
        assertEquals(3, obj.getLabel());
        assertEquals(4D, obj.getPosx(), 0.0D);
        assertEquals(5D, obj.getPosy(), 0.0D);
        assertEquals(6, obj.getInoutWeek());
        assertEquals(7, obj.getInoutWend());
        assertEquals(8D, obj.getRadiusWeek(), 0.0D);
        assertEquals(9D, obj.getDistCMWeek(), 0.0D);
        assertEquals(10D, obj.getRadiusWend(), 0.0D);
        assertEquals(11D, obj.getDistCMWend(), 0.0D);
    }
}
