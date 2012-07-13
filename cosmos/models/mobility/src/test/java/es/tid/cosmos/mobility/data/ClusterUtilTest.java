package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class ClusterUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ClusterUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Cluster> wrapper = ClusterUtil.createAndWrap(
                1, 2, 1, 0.57D, 30.12D, ClusterVector.getDefaultInstance());
        Cluster obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1, obj.getLabel());
        assertEquals(2, obj.getLabelgroup());
        assertEquals(1, obj.getConfident());
        assertEquals(0.57D, obj.getMean(), 0.0D);
        assertEquals(30.12D, obj.getDistance(), 0.0D);
        assertEquals(ClusterVector.getDefaultInstance(), obj.getCoords());
    }

    @Test
    public void testToString() {
        Cluster obj = ClusterUtil.create(1, 2, 1, 0.57D, 30.12D,
                                         ClusterVector.getDefaultInstance());
        assertEquals("1|2|1|0.57|30.12", ClusterUtil.toString(obj, "|"));
    }
}
