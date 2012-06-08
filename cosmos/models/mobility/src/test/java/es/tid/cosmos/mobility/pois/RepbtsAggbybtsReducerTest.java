package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 *
 * @author sortega
 */
public class RepbtsAggbybtsReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBtsDay>,
            ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>>
            driver;

    @Before
    public void setUp() {
        //this.driver = new ReduceDriver<ProtobufWritable<NodeBtsDay>,
        //        ProtobufWritable<MobData>, LongWritable,
        //        ProtobufWritable<MobData>>(new RepbtsAggbybtsReducer());
    }

    @Test
    public void testReduce() throws Exception {
        int node = 123;
        int bts = 456;
        int workday = 1;
        this.driver
                .withInput(NodeBtsDayUtil.createAndWrap(node, bts, workday, 101),
                           asList(MobDataUtil.createAndWrap(4),
                                  MobDataUtil.createAndWrap(5)))
                .withOutput(new LongWritable(node), MobDataUtil.createAndWrap(
                        NodeBtsDayUtil.create(node, bts, workday, 9)))
                .runTest();
    }
}
