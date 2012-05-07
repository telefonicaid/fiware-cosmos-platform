package es.tid.cosmos.mobility.data;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class NodeMxCounterUtilTest {
    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<NodeMxCounter> wrapper =
                NodeMxCounterUtil.createAndWrap(
                        asList(BtsCounter.getDefaultInstance(),
                               BtsCounter.getDefaultInstance()), 2, 3);
        NodeMxCounter obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(2, obj.getBtsCount());
        assertEquals(2, obj.getBtsLength());
        assertEquals(3, obj.getBtsMaxLength());
    }
}
