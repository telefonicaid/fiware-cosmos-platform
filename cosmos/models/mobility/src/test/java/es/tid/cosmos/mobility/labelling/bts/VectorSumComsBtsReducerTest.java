package es.tid.cosmos.mobility.labelling.bts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorSumComsBtsReducerTest {
    private ReduceDriver<ProtobufWritable<BtsCounter>,
            ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<BtsCounter>,
                ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
                ProtobufWritable<MobData>>(new VectorSumComsBtsReducer());
    }

    @Test
    public void testSomeMethod() {
        final ProtobufWritable<BtsCounter> inputKey =
                BtsCounterUtil.createAndWrap(1L, 2, 3, 4);
        final ProtobufWritable<NodeBts> outputKey =
                NodeBtsUtil.createAndWrap(0L, 1, 2, 0);
        final ProtobufWritable<MobData> inputValue = MobDataUtil.createAndWrap(
                NullWritable.get());
        final ProtobufWritable<MobData> outputValue = MobDataUtil.createAndWrap(
                TwoIntUtil.create(3, 7));
        this.driver
                .withInput(inputKey, asList(inputValue, inputValue, inputValue,
                                            inputValue, inputValue, inputValue,
                                            inputValue))
                .withOutput(outputKey, outputValue)
                .runTest();
    }
}
