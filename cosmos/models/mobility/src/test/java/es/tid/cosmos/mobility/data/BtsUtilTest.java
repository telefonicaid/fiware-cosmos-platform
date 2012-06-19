package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class BtsUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(BtsUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        long placeId = 132L;
        long comms = 50000L;
        double posx = 32D;
        double posy = 98D;
        double area = 157D;
        List<Long> adjBts = new ArrayList<Long>();
        adjBts.add(3L);
        adjBts.add(5L);
        adjBts.add(1L);

        ProtobufWritable<Bts> wrapper = BtsUtil.createAndWrap(
                placeId, comms, posx, posy, area, adjBts);
        wrapper.setConverter(Bts.class);
        Bts obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(placeId, obj.getPlaceId());
        assertEquals(comms, obj.getComms());
        assertEquals(posx, obj.getPosx(), 0.0D);
        assertEquals(posy, obj.getPosy(), 0.0D);
        assertEquals(area, obj.getArea(), 0.0D);
        assertEquals(3, obj.getAdjBtsCount());
    }
}
