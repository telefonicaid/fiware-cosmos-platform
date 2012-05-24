package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ExportClusterToTextReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, NullWritable,
            Text> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                NullWritable, Text>(new ExportClusterToTextReducer());
    }
    
    @Test
    public void testSetInputId() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final ClusterVector clusterVector = ClusterVector.newBuilder()
                .addAllComs(asList(6D, 7D, 8D)).build();
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 3, 4, 5, clusterVector));
        final Text outValue = new Text("57|1|2|3|4.0|5.0|6.0|7.0|8.0");
        this.driver
                .withInput(key, asList(value))
                .withOutput(NullWritable.get(), outValue)
                .runTest();
    }
}
