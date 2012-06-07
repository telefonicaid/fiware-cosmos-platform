package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;

/**
 *
 * @author dmicol
 */
public class ItinTimeUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ItinTime> wrapper = ItinTimeUtil.createAndWrap(
                Date.getDefaultInstance(), Time.getDefaultInstance(), 57L);
        wrapper.setConverter(ItinTime.class);
        ItinTime obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(Date.getDefaultInstance(), obj.getDate());
        assertEquals(Time.getDefaultInstance(), obj.getTime());
        assertEquals(57L, obj.getBts());
    }
}
