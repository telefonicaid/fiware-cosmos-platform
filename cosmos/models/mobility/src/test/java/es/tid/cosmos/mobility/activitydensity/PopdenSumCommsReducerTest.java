package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PopdenSumCommsReducerTest {
    private ReduceDriver<ProtobufWritable<BtsProfile>,
            ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
            ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<BtsProfile>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(new PopdenSumCommsReducer());
    }
    
    @Test
    public void testReduce() {
        final ProtobufWritable<BtsProfile> key = BtsProfileUtil.createAndWrap(
                1L, 2, 3, 4);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(57);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(102);
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(1L,
                                                                         2L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(0L, 3, 4, 159));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
