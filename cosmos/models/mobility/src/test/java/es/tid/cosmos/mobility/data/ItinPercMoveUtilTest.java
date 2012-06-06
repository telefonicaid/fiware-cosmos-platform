package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;

/**
 *
 * @author dmicol
 */
public class ItinPercMoveUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ItinPercMove> wrapper = ItinPercMoveUtil.createAndWrap(
                1, 2, 3.4D);
        wrapper.setConverter(ItinPercMove.class);
        ItinPercMove obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getGroup());
        assertEquals(2, obj.getRange());
        assertEquals(3.4D, obj.getPercMoves(), 0.0D);
    }
}
