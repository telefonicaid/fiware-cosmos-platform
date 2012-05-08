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
public class ClusterAggPotPoiPoisToPoiReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            LongWritable, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, LongWritable,
                ProtobufWritable<MobData>>(
                        new ClusterAggPotPoiPoisToPoiReducer());
    }

    @Test
    public void testChangeConfident() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(10, 20, 30, 40, 50, 0, 60, 70, 80, 0, 90, 100,
                               110, 0, 130, 140, 150));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(
                NullWritable.get());
        final LongWritable outKey = new LongWritable(3L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 1, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue)
                .runTest();
    }
    
    @Test
    public void testDontChangeConfident() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(10, 20, 30, 40, 50, 0, 60, 70, 80, 0, 90, 100,
                               110, 0, 130, 140, 150));
        final LongWritable outKey = new LongWritable(3L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
