package es.tid.ps.mobility.jobs;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.mobility.data.BtsCounterUtil;
import es.tid.ps.mobility.data.MxProtocol.BtsCounter;
import es.tid.ps.mobility.data.MxProtocol.NodeBts;
import es.tid.ps.mobility.data.NodeBtsUtil;

/**
 *
 * @author sortega
 */
public class MobilityNodeBtsCounterReducerTest {
    private MobmxNodeBtsCounterReducer instance;
    private ReduceDriver<
        ProtobufWritable<NodeBts>, NullWritable, LongWritable,
        ProtobufWritable<BtsCounter>> driver;

    @Before
    public void setUp() {
        this.instance = new MobmxNodeBtsCounterReducer();
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
