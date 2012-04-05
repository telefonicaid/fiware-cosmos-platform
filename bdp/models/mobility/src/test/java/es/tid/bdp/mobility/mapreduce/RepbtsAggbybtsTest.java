package es.tid.bdp.mobility.mapreduce;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.bdp.mobility.data.NodeBtsDayUtil;

/**
 *
 * @author sortega
 */
public class RepbtsAggbybtsTest {
    private ReduceDriver<ProtobufWritable<NodeBtsDay>,
        IntWritable, LongWritable, ProtobufWritable<NodeBtsDay>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBtsDay>,
                IntWritable, LongWritable, ProtobufWritable<NodeBtsDay>>(
                new RepbtsAggbybtsReducer());
    }

    @Test
    public void reduceTest() throws Exception {
        int node = 123;
        int bts = 456;
        int workday = 1;

        this.driver
                .withInput(NodeBtsDayUtil.createAndWrap(node, bts, workday, 101),
                           asList(new IntWritable(4), new IntWritable(5)))
                .withOutput(new LongWritable(node),
                            NodeBtsDayUtil.createAndWrap(node, bts, workday, 9))
                .runTest();
    }
}
