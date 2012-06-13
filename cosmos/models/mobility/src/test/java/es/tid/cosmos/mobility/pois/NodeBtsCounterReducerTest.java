package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;

/**
 *
 * @author sortega
 */
public class NodeBtsCounterReducerTest {
    private ReduceDriver<
        ProtobufWritable<NodeBts>, TypedProtobufWritable<Null>, LongWritable,
        TypedProtobufWritable<BtsCounter>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                TypedProtobufWritable<Null>, LongWritable,
                TypedProtobufWritable<BtsCounter>>(new NodeBtsCounterReducer());
    }


    @Test
    public void reduceTest() throws Exception {
        int phone = 1234;
        TypedProtobufWritable<BtsCounter> expectedBtsCounter =
                new TypedProtobufWritable<BtsCounter>(
                        BtsCounterUtil.create(1, 2, 3, 2));
        final TypedProtobufWritable<Null> nullValue = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(NodeBtsUtil.createAndWrap(phone, 1, 2, 3),
                           asList(nullValue, nullValue))
                .withOutput(new LongWritable(phone), expectedBtsCounter)
                .runTest();
    }
}
