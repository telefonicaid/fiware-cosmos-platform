package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PopdenSumCommsReducerTest {
    private ReduceDriver<ProtobufWritable<BtsProfile>,
            MobilityWritable<Int>, ProtobufWritable<TwoInt>,
            MobilityWritable<BtsCounter>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<BtsProfile>,
                MobilityWritable<Int>, ProtobufWritable<TwoInt>,
                MobilityWritable<BtsCounter>>(new PopdenSumCommsReducer());
    }
    
    @Test
    public void testReduce() {
        final ProtobufWritable<BtsProfile> key = BtsProfileUtil.createAndWrap(
                1L, 2, 3, 4);
        final MobilityWritable<Int> value1 = MobilityWritable.create(57);
        final MobilityWritable<Int> value2 = MobilityWritable.create(102);
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(1L,
                                                                         2L);
        final MobilityWritable<BtsCounter> outValue = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(0L, 3, 4, 159));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
