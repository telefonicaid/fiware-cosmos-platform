package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class ClusterJoinPotPoiLabelReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>(
                        new ClusterJoinPotPoiLabelReducer());
    }

    @Test
    public void shouldProduceOneOutput() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(10L);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(32L);
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                NullWritable.get());
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue)
                .runTest();
    }
    
    @Test
    public void shouldProduceTwoOutputs() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(10L);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                NullWritable.get());
        this.driver
                .withInput(key, asList(value1, value2, value1))
                .withOutput(outKey, outValue)
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputs() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(55L);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(32L);
        this.driver
                .withInput(key, asList(value1, value2, value1))
                .runTest();
    }
}
