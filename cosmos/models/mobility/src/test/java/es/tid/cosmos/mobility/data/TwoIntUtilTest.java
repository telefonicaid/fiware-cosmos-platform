package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class TwoIntUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<TwoInt> wrapper = TwoIntUtil.createAndWrap(3L, 5L);
        wrapper.setConverter(TwoInt.class);
        TwoInt obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(3L, obj.getNum1());
        assertEquals(5L, obj.getNum2());
    }

    @Test
    public void testGetPartition() {
        TwoInt obj = TwoIntUtil.create(3L, 5L);
        assertEquals(1, TwoIntUtil.getPartition(obj, 2));
    }

    @Test
    public void testToString() {
        TwoInt obj = TwoIntUtil.create(3L, 5L);
        assertEquals("3|5", TwoIntUtil.toString(obj, "\\|"));
    }
}
