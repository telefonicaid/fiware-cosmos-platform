package es.tid.cosmos.mobility.labelling.clientbts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;

/**
 *
 * @author dmicol
 */
public class VectorSpreadNodbtsReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<NodeBts>, ProtobufWritable<MobData>>(
                        new VectorSpreadNodbtsReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final BtsCounter btsCounter1 = BtsCounterUtil.create(1, 2, 3, 4);
        final BtsCounter btsCounter2 = BtsCounterUtil.create(2, 0, 3, 4);
        final BtsCounter btsCounter3 = BtsCounterUtil.create(3, 5, 3, 4);
        final BtsCounter btsCounter4 = BtsCounterUtil.create(4, 6, 3, 4);
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                NodeMxCounterUtil.create(asList(btsCounter1, btsCounter2,
                                                btsCounter3, btsCounter4),
                                         2, 3));
        final ProtobufWritable<NodeBts> outKey1 = NodeBtsUtil.createAndWrap(
                57L, 1, 0, 3);
        final ProtobufWritable<MobData> outValue1 = MobDataUtil.createAndWrap(
                4);
        final ProtobufWritable<NodeBts> outKey2 = NodeBtsUtil.createAndWrap(
                57L, 2, 3, 3);
        final ProtobufWritable<MobData> outValue2 = MobDataUtil.createAndWrap(
                4);
        final ProtobufWritable<NodeBts> outKey3 = NodeBtsUtil.createAndWrap(
                57L, 3, 1, 3);
        final ProtobufWritable<MobData> outValue3 = MobDataUtil.createAndWrap(
                4);
        final ProtobufWritable<NodeBts> outKey4 = NodeBtsUtil.createAndWrap(
                57L, 4, 2, 3);
        final ProtobufWritable<MobData> outValue4 = MobDataUtil.createAndWrap(
                4);
        this.driver
                .withInput(key, asList(value))
                .withOutput(outKey1, outValue1)
                .withOutput(outKey2, outValue2)
                .withOutput(outKey3, outValue3)
                .withOutput(outKey4, outValue4)
                .runTest();
    }
}
