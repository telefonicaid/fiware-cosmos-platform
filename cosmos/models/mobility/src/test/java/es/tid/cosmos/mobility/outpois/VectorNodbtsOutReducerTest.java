package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorNodbtsOutReducerTest extends ConfiguredTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>,
            NullWritable, Text> driver;

    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Cluster>, NullWritable, Text>(
                        new VectorNodbtsOutReducer());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void testReduce() throws IOException {
        ClusterVector clusterVector1 = ClusterVector.newBuilder()
                .addAllComs(asList(0.4D, 5.6D, 3.2D))
                .build();
        ClusterVector clusterVector2 = ClusterVector.newBuilder()
                .addAllComs(asList(0.4D, 3.2D))
                .build();
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Cluster> value1 = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(1, 2, 3, 4, 5, clusterVector1));
        final TypedProtobufWritable<Cluster> value2 = new TypedProtobufWritable<Cluster>(
                ClusterUtil.create(10, 20, 30, 40, 50, clusterVector2));
        List<Pair<NullWritable, Text>> results = this.driver
                .withInput(key, asList(value1, value2))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        Text firstResult = results.get(0).getSecond();
        assertEquals(8, firstResult.toString().split("\\|").length);
        Text secondResult = results.get(1).getSecond();
        assertEquals(7, secondResult.toString().split("\\|").length);
    }
}
