package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjJoinPairbtsAdjbtsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            LongWritable, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, LongWritable,
                ProtobufWritable<MobData>>(new AdjJoinPairbtsAdjbtsReducer());
    }

    @Test
    public void testWithOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        MobData value1 = MobDataUtil.create(TwoIntUtil.create(57L, 32L));
        value1 = MobDataUtil.setInputId(value1, 0);
        MobData value2 = MobDataUtil.create(TwoIntUtil.create(157L, 132L));
        value2 = MobDataUtil.setInputId(value2, 1);
        final LongWritable outKey = new LongWritable(57L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                TwoIntUtil.create(57L, 32L));
        this.driver
                .withInput(key, asList(MobDataUtil.wrap(value1),
                                       MobDataUtil.wrap(value2)))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void testNoOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        MobData value1 = MobDataUtil.create(TwoIntUtil.create(57L, 32L));
        value1 = MobDataUtil.setInputId(value1, 0);
        this.driver
                .withInput(key, asList(MobDataUtil.wrap(value1)))
                .runTest();
    }
}
