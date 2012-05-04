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
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class PoiFilterSechomeAdjacentReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(
                        new PoiFilterSechomeAdjacentReducer());
    }

    @Test
    public void testWithOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(17L);
        final ProtobufWritable<TwoInt> outKey1 = TwoIntUtil.createAndWrap(17L,
                                                                          32L);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(88L);
        final ProtobufWritable<TwoInt> outKey2 = TwoIntUtil.createAndWrap(88L,
                                                                          32L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                NullWritable.get());
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey1, outValue)
                .withOutput(outKey2, outValue)
                .runTest();
    }
    
    @Test
    public void testEmptyOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(17L);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                NullWritable.get());
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(88L);
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .runTest();
    }
}
