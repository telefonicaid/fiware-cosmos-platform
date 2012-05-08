package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.PoiUtil;

/**
 *
 * @author dmicol
 */
public class ClusterAggNodeClusterByNodlblReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            LongWritable, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new ClusterAggNodeClusterByNodlblReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final LongWritable outKey = new LongWritable(1L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 1, 2, 0, 4, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
