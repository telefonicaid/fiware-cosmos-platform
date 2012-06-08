package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author sortega
 */
public class NodeBtsCounterReducerTest {
    private ReduceDriver<
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        //this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
        //        ProtobufWritable<MobData>, LongWritable,
        //        ProtobufWritable<MobData>>(new NodeBtsCounterReducer());
    }


    @Test
    public void reduceTest() throws Exception {
        int phone = 1234;
        ProtobufWritable<MobData> expectedBtsCounter =
                MobDataUtil.createAndWrap(BtsCounterUtil.create(1, 2, 3, 2));

        this.driver
                .withInput(NodeBtsUtil.createAndWrap(phone, 1, 2, 3),
                           asList(MobDataUtil.createAndWrap(NullWritable.get()),
                                  MobDataUtil.createAndWrap(NullWritable.get())))
                .withOutput(new LongWritable(phone), expectedBtsCounter)
                .runTest();
    }
}
