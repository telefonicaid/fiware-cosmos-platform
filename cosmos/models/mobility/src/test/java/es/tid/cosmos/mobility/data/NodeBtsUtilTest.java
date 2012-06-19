package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class NodeBtsUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(NodeBtsUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        long userId = 132L;
        int placeId = 81;
        int weekday = 5;
        int range = 3;
        ProtobufWritable<NodeBts> wrapper = NodeBtsUtil.createAndWrap(
                userId, placeId, weekday, range);
        wrapper.setConverter(NodeBts.class);
        NodeBts obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(userId, obj.getUserId());
        assertEquals(placeId, obj.getBts());
        assertEquals(weekday, obj.getWeekday());
        assertEquals(range, obj.getRange());
    }
}
