package es.tid.cosmos.mobility.labelling.secondhomes;

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
public class PoiJoinSechomeResultsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(new PoiJoinSechomeResultsReducer());
    }

    @Test
    public void testOutputWithSecHomeCount() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(52L, 37L));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(
                NullWritable.get());
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 100, 100, 14,
                               15, 52, 37));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testOutputWithoutSecHomeCount() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(52L, 37L));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 52, 37));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
