package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector.Builder;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author ximo
 */
public class VectorNormalizedMapperTest {

    private MapDriver<ProtobufWritable<NodeBts>,
            TypedProtobufWritable<ClusterVector>, ProtobufWritable<NodeBts>,
            TypedProtobufWritable<ClusterVector>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<NodeBts>,
                TypedProtobufWritable<ClusterVector>, ProtobufWritable<NodeBts>,
                TypedProtobufWritable<ClusterVector>>(
                        new VectorNormalizedMapper());
    }

    @Test
    public void testMap() throws IOException {
        final ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(1L, 2L,
                                                                        3, 4);
        double sum = 0.0D;
        Builder vectorBuilder = ClusterVector.newBuilder();
        for (int i = 0; i < 1000; i++) {
            final double value = 3.141592D * i;
            vectorBuilder.addComs(value);
            sum += value;
        }
        List<Pair<ProtobufWritable<NodeBts>,
                  TypedProtobufWritable<ClusterVector>>> results =
                this.driver.withInput(key, new TypedProtobufWritable<ClusterVector>(
                        vectorBuilder.build())).run();
        assertEquals(results.size(), 1);
        ClusterVector resultVector = results.get(0).getSecond().get();
        double total = 0.0D;
        for (double value : resultVector.getComsList()) {
            total += value;
        }
        assertEquals(total, 1.0D, 1e-9D);
    }
}
