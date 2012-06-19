package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class NodeMxCounterUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(NodeMxCounterUtil.class);
    }

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
