package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;

/**
 *
 * @author dmicol
 */
public class DateUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Date> wrapper = DateUtil.createAndWrap(2012, 4, 20, 5);
        Date obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(2012, obj.getYear());
        assertEquals(4, obj.getMonth());
        assertEquals(20, obj.getDay());
        assertEquals(5, obj.getWeekday());
    }

    @Test
    public void testParse() {
        Date obj = DateUtil.parse("2012|4|20|5", "\\|");
        assertEquals(2012, obj.getYear());
        assertEquals(4, obj.getMonth());
        assertEquals(20, obj.getDay());
        assertEquals(5, obj.getWeekday());
    }

    @Test
    public void testToString() {
        Date obj = DateUtil.create(2012, 4, 20, 5);
        assertEquals("2012|4|20|5", DateUtil.toString(obj, "|"));
    }
}
