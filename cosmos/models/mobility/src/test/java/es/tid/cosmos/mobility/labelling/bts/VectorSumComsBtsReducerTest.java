package es.tid.cosmos.mobility.labelling.bts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorSumComsBtsReducerTest {
    private ReduceDriver<ProtobufWritable<BtsCounter>, TypedProtobufWritable<Null>,
            ProtobufWritable<NodeBts>, TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<BtsCounter>,
                TypedProtobufWritable<Null>, ProtobufWritable<NodeBts>,
                TypedProtobufWritable<TwoInt>>(new VectorSumComsBtsReducer());
    }

    @Test
    public void testSomeMethod() {
        final ProtobufWritable<BtsCounter> inputKey =
                BtsCounterUtil.createAndWrap(1L, 2, 3, 4);
        final ProtobufWritable<NodeBts> outputKey =
                NodeBtsUtil.createAndWrap(0L, 1, 2, 0);
        final TypedProtobufWritable<Null> inputValue = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        final TypedProtobufWritable<TwoInt> outputValue = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(3, 7));
        this.driver
                .withInput(inputKey, asList(inputValue, inputValue, inputValue,
                                            inputValue, inputValue, inputValue,
                                            inputValue))
                .withOutput(outputKey, outputValue)
                .runTest();
    }
}
