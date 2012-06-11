package es.tid.cosmos.mobility.activitydensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class PopdenSpreadArrayReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<NodeMxCounter>,
            ProtobufWritable<BtsProfile>, MobilityWritable<Int>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                MobilityWritable<NodeMxCounter>, ProtobufWritable<BtsProfile>,
                MobilityWritable<Int>>(new PopdenSpreadArrayReducer());
    }
    
    @Test
    public void testReduce() {
        final BtsCounter counter1 = BtsCounterUtil.create(1L, 2, 3, 4);
        final BtsCounter counter2 = BtsCounterUtil.create(5L, 6, 7, 8);
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<NodeMxCounter> value = new MobilityWritable<NodeMxCounter>(
                NodeMxCounterUtil.create(Arrays.asList(counter1, counter2),
                                         2, 3));
        final ProtobufWritable<BtsProfile> outKey1 =
                BtsProfileUtil.createAndWrap(1L, 0, 2, 3);
        final MobilityWritable<Int> outValue1 = MobilityWritable.create(4);
        final ProtobufWritable<BtsProfile> outKey2 =
                BtsProfileUtil.createAndWrap(5L, 0, 6, 7);
        final MobilityWritable<Int> outValue2 = MobilityWritable.create(8);
        this.instance
                .withInput(key, Arrays.asList(value))
                .withOutput(outKey1, outValue1)
                .withOutput(outKey2, outValue2)
                .runTest();
    }
}
