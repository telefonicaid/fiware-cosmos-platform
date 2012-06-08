package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class NodeMobInfoReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        //this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
        //        LongWritable, ProtobufWritable<MobData>>(
        //                new NodeMobInfoReducer());
    }

    @Test
    public void testReduce() throws Exception {
        LongWritable key = new LongWritable(57L);
        ProtobufWritable<MobData> btsCounter1 = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(1L, 2, 3, 4));
        ProtobufWritable<MobData> btsCounter2 = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(5L, 6, 7, 8));
        ProtobufWritable<MobData> output = MobDataUtil.createAndWrap(
                NodeMxCounterUtil.create(
                        asList(btsCounter1.get().getBtsCounter(),
                               btsCounter2.get().getBtsCounter()), 2, 2));
        this.driver
                .withInput(key, asList(btsCounter1, btsCounter2))
                .withOutput(key, output)
                .runTest();
    }
}
