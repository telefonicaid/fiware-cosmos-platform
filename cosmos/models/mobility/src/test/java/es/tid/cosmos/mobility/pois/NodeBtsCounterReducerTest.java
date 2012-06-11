package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;

/**
 *
 * @author sortega
 */
public class NodeBtsCounterReducerTest {
    private ReduceDriver<
        ProtobufWritable<NodeBts>, MobilityWritable<Null>, LongWritable,
        MobilityWritable<BtsCounter>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                MobilityWritable<Null>, LongWritable,
                MobilityWritable<BtsCounter>>(new NodeBtsCounterReducer());
    }


    @Test
    public void reduceTest() throws Exception {
        int phone = 1234;
        MobilityWritable<BtsCounter> expectedBtsCounter =
                new MobilityWritable<BtsCounter>(
                        BtsCounterUtil.create(1, 2, 3, 2));
        final MobilityWritable<Null> nullValue = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(NodeBtsUtil.createAndWrap(phone, 1, 2, 3),
                           asList(nullValue, nullValue))
                .withOutput(new LongWritable(phone), expectedBtsCounter)
                .runTest();
    }
}
