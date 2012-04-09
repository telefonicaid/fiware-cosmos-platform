package es.tid.cosmos.mobility.mapreduce;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.mapreduce.NodeBtsCounterReducer;

/**
 *
 * @author sortega
 */
public class MobilityNodeBtsCounterReducerTest {
    private NodeBtsCounterReducer instance;
    private ReduceDriver<
        ProtobufWritable<NodeBts>, NullWritable, LongWritable,
        ProtobufWritable<BtsCounter>> driver;

    @Before
    public void setUp() {
        this.instance = new NodeBtsCounterReducer();
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>, NullWritable,
                LongWritable, ProtobufWritable<BtsCounter>>(this.instance);
    }


    @Test
    public void reduceTest() throws Exception {
        int phone = 1234;
        ProtobufWritable<BtsCounter> expectedBtsCounter =
                BtsCounterUtil.createAndWrap(1, 2, 3, 2);

        this.driver
                .withInput(NodeBtsUtil.createAndWrap(phone, 1, 2, 3),
                           asList(NullWritable.get(), NullWritable.get()))
                .withOutput(new LongWritable(phone), expectedBtsCounter)
                .runTest();
    }
}
