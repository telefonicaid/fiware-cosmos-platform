package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;

/**
 *
 * @author dmicol
 */
public class BtsCounterUtilTest {
    @Test
    public void testCreateAndWrap() {
        long placeId = 132L;
        int weekday = 4;
        int range = 2;
        int count = 9;
        ProtobufWritable<BtsCounter> wrapper = BtsCounterUtil.createAndWrap(
                placeId, weekday, range, count);
        wrapper.setConverter(BtsCounter.class);
        BtsCounter obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(placeId, obj.getBts());
        assertEquals(weekday, obj.getWeekday());
        assertEquals(range, obj.getRange());
        assertEquals(count, obj.getCount());
    }
}
