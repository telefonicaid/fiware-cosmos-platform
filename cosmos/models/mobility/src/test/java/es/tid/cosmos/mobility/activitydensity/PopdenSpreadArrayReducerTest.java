package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class PopdenSpreadArrayReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<NodeMxCounter>,
            ProtobufWritable<BtsProfile>, TypedProtobufWritable<Int>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<NodeMxCounter>, ProtobufWritable<BtsProfile>,
                TypedProtobufWritable<Int>>(new PopdenSpreadArrayReducer());
    }
    
    @Test
    public void testReduce() {
        final BtsCounter counter1 = BtsCounterUtil.create(1L, 2, 3, 4);
        final BtsCounter counter2 = BtsCounterUtil.create(5L, 6, 7, 8);
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<NodeMxCounter> value = new TypedProtobufWritable<NodeMxCounter>(
                NodeMxCounterUtil.create(Arrays.asList(counter1, counter2),
                                         2, 3));
        final ProtobufWritable<BtsProfile> outKey1 =
                BtsProfileUtil.createAndWrap(1L, 0, 2, 3);
        final TypedProtobufWritable<Int> outValue1 = TypedProtobufWritable.create(4);
        final ProtobufWritable<BtsProfile> outKey2 =
                BtsProfileUtil.createAndWrap(5L, 0, 6, 7);
        final TypedProtobufWritable<Int> outValue2 = TypedProtobufWritable.create(8);
        this.instance
                .withInput(key, Arrays.asList(value))
                .withOutput(outKey1, outValue1)
                .withOutput(outKey2, outValue2)
                .runTest();
    }
}
