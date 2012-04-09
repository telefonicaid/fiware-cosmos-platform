package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class TwoIntUtilTest {
    @Test
    public void testCreateAndWrap() {
        long num1 = 3L;
        long num2 = 5L;
        ProtobufWritable<TwoInt> wrapper = TwoIntUtil.createAndWrap(num1, num2);
        wrapper.setConverter(TwoInt.class);
        TwoInt obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(num1, obj.getNum1());
        assertEquals(num2, obj.getNum2());
    }
}
