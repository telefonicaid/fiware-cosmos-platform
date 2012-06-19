package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class TelMonthUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(TelMonthUtil.class);
    }

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
