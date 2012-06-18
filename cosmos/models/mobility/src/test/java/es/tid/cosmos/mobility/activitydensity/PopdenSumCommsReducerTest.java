package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PopdenSumCommsReducerTest {
    private ReduceDriver<ProtobufWritable<BtsProfile>,
            TypedProtobufWritable<Int>, ProtobufWritable<TwoInt>,
            TypedProtobufWritable<BtsCounter>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<BtsProfile>,
                TypedProtobufWritable<Int>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<BtsCounter>>(new PopdenSumCommsReducer());
    }
    
    @Test
    public void testReduce() {
        final ProtobufWritable<BtsProfile> key = BtsProfileUtil.createAndWrap(
                1L, 2, 3, 4);
        final TypedProtobufWritable<Int> value1 = TypedProtobufWritable.create(57);
        final TypedProtobufWritable<Int> value2 = TypedProtobufWritable.create(102);
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(1L,
                                                                         2L);
        final TypedProtobufWritable<BtsCounter> outValue = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(0L, 3, 4, 159));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
