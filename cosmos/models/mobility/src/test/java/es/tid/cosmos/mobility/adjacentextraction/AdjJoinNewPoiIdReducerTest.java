package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjJoinNewPoiIdReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>(
                        new AdjJoinNewPoiIdReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(103L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(57L);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiNewUtil.create(1, 2, 3, 4, 0));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(32L);
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final ProtobufWritable<MobData> outValue1 = MobDataUtil.createAndWrap(
                PoiNewUtil.create(57, 2, 3, 4, 0));
        final ProtobufWritable<MobData> outValue2 = MobDataUtil.createAndWrap(
                PoiNewUtil.create(32, 2, 3, 4, 0));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue1)
                .withOutput(outKey, outValue2)
                .runTest();
    }
}
