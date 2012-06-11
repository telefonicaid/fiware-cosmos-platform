package es.tid.cosmos.mobility.labelling.bts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorSumComsBtsReducerTest {
    private ReduceDriver<ProtobufWritable<BtsCounter>, MobilityWritable<Null>,
            ProtobufWritable<NodeBts>, MobilityWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<BtsCounter>,
                MobilityWritable<Null>, ProtobufWritable<NodeBts>,
                MobilityWritable<TwoInt>>(new VectorSumComsBtsReducer());
    }

    @Test
    public void testSomeMethod() {
        final ProtobufWritable<BtsCounter> inputKey =
                BtsCounterUtil.createAndWrap(1L, 2, 3, 4);
        final ProtobufWritable<NodeBts> outputKey =
                NodeBtsUtil.createAndWrap(0L, 1, 2, 0);
        final MobilityWritable<Null> inputValue = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        final MobilityWritable<TwoInt> outputValue = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(3, 7));
        this.driver
                .withInput(inputKey, asList(inputValue, inputValue, inputValue,
                                            inputValue, inputValue, inputValue,
                                            inputValue))
                .withOutput(outputKey, outputValue)
                .runTest();
    }
}
