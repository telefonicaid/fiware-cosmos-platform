package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ExportClusterToTextByTwoIntReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<Cluster>,
            NullWritable, Text> driver;
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<Cluster>, NullWritable, Text>(
                        new ExportClusterToTextByTwoIntReducer());
    }
    
    @Test
    public void testSetInputId() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ClusterVector clusterVector = ClusterVector.newBuilder()
                .addAllComs(asList(6D, 7D, 8D)).build();
        final MobilityWritable<Cluster> value = new MobilityWritable<Cluster>(
                ClusterUtil.create(1, 2, 3, 4, 5, clusterVector));
        final Text outValue = new Text("57|32|1|2|3|4.0|5.0|6.0|7.0|8.0");
        this.driver
                .withInput(key, asList(value))
                .withOutput(NullWritable.get(), outValue)
                .runTest();
    }
}
