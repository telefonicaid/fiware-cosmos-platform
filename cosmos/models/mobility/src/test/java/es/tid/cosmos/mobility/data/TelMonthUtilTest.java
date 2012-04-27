package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;

/**
 *
 * @author dmicol
 */
public class TelMonthUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<TelMonth> wrapper = TelMonthUtil.createAndWrap(1L, 2,
                                                                        true);
        TelMonth obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getPhone());
        assertEquals(2, obj.getMonth());
        assertEquals(true, obj.getWorkingday());
    }
}
