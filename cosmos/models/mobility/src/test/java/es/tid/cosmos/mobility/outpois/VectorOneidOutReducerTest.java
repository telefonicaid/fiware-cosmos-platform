package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class VectorOneidOutReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, NullWritable,
            Text> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                NullWritable, Text>(new VectorOneidOutReducer());
    }

    @Test
    public void testReduce() throws IOException {
        ClusterVector clusterVector1 = ClusterVector.newBuilder()
                .addAllComs(asList(0.4D, 5.6D, 3.2D))
                .build();
        ClusterVector clusterVector2 = ClusterVector.newBuilder()
                .addAllComs(asList(0.4D, 3.2D))
                .build();
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 3, 4, 5, clusterVector1));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(10, 20, 30, 40, 50, clusterVector2));
        List<Pair<NullWritable, Text>> results = this.driver
                .withInput(new LongWritable(57L), asList(value1, value2))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        Text firstResult = results.get(0).getSecond();
        assertEquals(7, firstResult.toString().split("\\|").length);
        Text secondResult = results.get(1).getSecond();
        assertEquals(6, secondResult.toString().split("\\|").length);
    }
}
