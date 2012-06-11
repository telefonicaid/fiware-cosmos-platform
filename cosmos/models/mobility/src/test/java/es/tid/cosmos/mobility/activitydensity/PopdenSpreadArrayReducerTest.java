package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class PopdenSpreadArrayReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<BtsProfile>, ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                ProtobufWritable<MobData>, ProtobufWritable<BtsProfile>,
                ProtobufWritable<MobData>>(new PopdenSpreadArrayReducer());
    }
    
    @Test
    public void testReduce() {
        final BtsCounter counter1 = BtsCounterUtil.create(1L, 2, 3, 4);
        final BtsCounter counter2 = BtsCounterUtil.create(5L, 6, 7, 8);
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                NodeMxCounterUtil.create(Arrays.asList(counter1, counter2),
                                         2, 3));
        final ProtobufWritable<BtsProfile> outKey1 =
                BtsProfileUtil.createAndWrap(1L, 0, 2, 3);
        final ProtobufWritable<MobData> outValue1 =
                MobDataUtil.createAndWrap(4);
        final ProtobufWritable<BtsProfile> outKey2 =
                BtsProfileUtil.createAndWrap(5L, 0, 6, 7);
        final ProtobufWritable<MobData> outValue2 =
                MobDataUtil.createAndWrap(8);
        this.instance
                .withInput(key, Arrays.asList(value))
                .withOutput(outKey1, outValue1)
                .withOutput(outKey2, outValue2)
                .runTest();
    }
}
