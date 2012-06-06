package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;

/**
 *
 * @author dmicol
 */
public class MatrixTimeUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<MatrixTime> wrapper = MatrixTimeUtil.createAndWrap(
                Date.getDefaultInstance(), Time.getDefaultInstance(), 102, 57L);
        wrapper.setConverter(MatrixTime.class);
        MatrixTime obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(Date.getDefaultInstance(), obj.getDate());
        assertEquals(Time.getDefaultInstance(), obj.getTime());
        assertEquals(102, obj.getGroup());
        assertEquals(57L, obj.getBts());
    }
}
