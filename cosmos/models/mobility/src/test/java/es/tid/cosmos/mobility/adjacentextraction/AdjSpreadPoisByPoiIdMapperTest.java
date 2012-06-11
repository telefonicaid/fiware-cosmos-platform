package es.tid.cosmos.mobility.adjacentextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadPoisByPoiIdMapperTest {
    private MapDriver<ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>,
            LongWritable, MobilityWritable<PoiNew>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<PoiNew>, LongWritable,
                MobilityWritable<PoiNew>>(new AdjSpreadPoisByPoiIdMapper());
    }

    @Test
    public void testMap() {
        final MobilityWritable<PoiNew> poiNew = new MobilityWritable<PoiNew>(
                PoiNewUtil.create(1, 2L, 3L, 4, 0));
        this.driver
                .withInput(TwoIntUtil.createAndWrap(5L, 6L), poiNew)
                .withOutput(new LongWritable(1L), poiNew)
                .runTest();
    }
}
