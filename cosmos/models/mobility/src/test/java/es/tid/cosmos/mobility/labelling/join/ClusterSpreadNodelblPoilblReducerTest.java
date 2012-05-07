package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
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
public class ClusterSpreadNodelblPoilblReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(
                        new ClusterSpreadNodelblPoilblReducer());
    }
    
    @Test
    public void testTwoOutputs() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 1, 6, 7, 8, 9, 10, 11, 12, 1, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey1 = TwoIntUtil.createAndWrap(4,
                                                                          11);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(10, 20, 30, 40, 50, 1, 60, 70, 80, 90, 100, 110,
                               120, 1, 130, 140, 150));
        final ProtobufWritable<TwoInt> outKey2 = TwoIntUtil.createAndWrap(40,
                                                                          110);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                NullWritable.get());
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey1, outValue)
                .withOutput(outKey2, outValue)
                .runTest();
    }

    @Test
    public void testOneOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 1, 6, 7, 8, 9, 10, 11, 12, 0, 13,
                               14, 15));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(10, 20, 30, 40, 50, 1, 60, 70, 80, 90, 100, 110,
                               120, 1, 130, 140, 150));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 9, 10, 11, 12, 1, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(40,
                                                                         110);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                NullWritable.get());
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue)
                .runTest();
    }
    
    @Test
    public void testNoOutputs() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 9, 10, 11, 12, 0, 13,
                               14, 15));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiUtil.create(10, 20, 30, 40, 50, 0, 60, 70, 80, 90, 100, 110,
                               120, 0, 130, 140, 150));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }
}
