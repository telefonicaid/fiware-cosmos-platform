package es.tid.cosmos.mobility.labelling.secondhomes;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
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
public class PoiJoinPoisBtscoordToPoiMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>(
                        new PoiJoinPoisBtscoordToPoiMapper());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                               11, 12, 13, 14, 15, 16, 17));
        this.driver
                .withInput(new LongWritable(57L), value)
                .withOutput(TwoIntUtil.createAndWrap(2, 3), value)
                .runTest();
    }
}
