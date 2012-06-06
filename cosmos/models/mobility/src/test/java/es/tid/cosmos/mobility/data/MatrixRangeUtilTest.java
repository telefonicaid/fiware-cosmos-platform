package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;

/**
 *
 * @author dmicol
 */
public class MatrixRangeUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<MatrixRange> wrapper = MatrixRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        wrapper.setConverter(MatrixRange.class);
        MatrixRange obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getNode());
        assertEquals(2, obj.getPoiSrc());
        assertEquals(3, obj.getPoiTgt());
        assertEquals(4, obj.getGroup());
        assertEquals(5, obj.getRange());
    }
}
