package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class TimeUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(TimeUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Time> wrapper = TimeUtil.createAndWrap(1, 2, 3);
        Time obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getHour());
        assertEquals(2, obj.getMinute());
        assertEquals(3, obj.getSeconds());
    }

    @Test
    public void testToString() {
        Time obj = TimeUtil.create(1, 2, 3);
        assertNotNull(obj);
        assertEquals("1|2|3", TimeUtil.toString(obj, "|"));
    }
}