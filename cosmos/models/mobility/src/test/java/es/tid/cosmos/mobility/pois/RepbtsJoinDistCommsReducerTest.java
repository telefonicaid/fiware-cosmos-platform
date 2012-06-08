package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class RepbtsJoinDistCommsReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        //this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
        //        LongWritable, ProtobufWritable<MobData>>(
        //                new RepbtsJoinDistCommsReducer());
    }

    @Test
    public void testReduce() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> nodeBtsDay1 = MobDataUtil.createAndWrap(
                NodeBtsDayUtil.create(1L, 2L, 3, 4));
        final ProtobufWritable<MobData> nodeBtsDay2 = MobDataUtil.createAndWrap(
                NodeBtsDayUtil.create(5L, 6L, 7, 8));
        final ProtobufWritable<MobData> int1 = MobDataUtil.createAndWrap(37);
        final ProtobufWritable<MobData> int2 = MobDataUtil.createAndWrap(132);
        final ProtobufWritable<MobData> output1 = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(2L, 0, 4, 10));
        final ProtobufWritable<MobData> output2 = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(6L, 0, 8, 21));
        final ProtobufWritable<MobData> output3 = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(2L, 0, 4, 3));
        final ProtobufWritable<MobData> output4 = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(6L, 0, 8, 6));
        this.driver
                .withInput(key, asList(int1, nodeBtsDay1, nodeBtsDay2, int2))
                .withOutput(key, output1)
                .withOutput(key, output2)
                .withOutput(key, output3)
                .withOutput(key, output4)
                .runTest();
    }
}
