package es.tid.cosmos.mobility.labelling.bts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsUtil;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class FilterBtsVectorReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        Config.maxBtsArea = 10.83515D;
        Config.maxCommsBts = 70000L;
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new FilterBtsVectorReducer());
    }

    @Test
    public void testNoConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                BtsUtil.create(1, 50000, 2, 3, 4, asList(5L, 6L, 7L)));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                BtsUtil.create(1, 80000, 2, 3, 4, asList(5L, 6L, 7L)));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
