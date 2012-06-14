package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class NodeMobInfoReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<BtsCounter>,
            LongWritable, TypedProtobufWritable<NodeMxCounter>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<BtsCounter>,
                LongWritable, TypedProtobufWritable<NodeMxCounter>>(
                        new NodeMobInfoReducer());
    }

    @Test
    public void testReduce() throws Exception {
        LongWritable key = new LongWritable(57L);
        TypedProtobufWritable<BtsCounter> btsCounter1 = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(1L, 2, 3, 4));
        TypedProtobufWritable<BtsCounter> btsCounter2 = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(5L, 6, 7, 8));
        TypedProtobufWritable<NodeMxCounter> output = new TypedProtobufWritable<NodeMxCounter>(
                NodeMxCounterUtil.create(
                        asList(btsCounter1.get(), btsCounter2.get()), 2, 2));
        this.driver
                .withInput(key, asList(btsCounter1, btsCounter2))
                .withOutput(key, output)
                .runTest();
    }
}
