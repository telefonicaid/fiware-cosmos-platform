package es.tid.cosmos.mobility.adjacentextraction;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapperTest {
    private MapDriver<LongWritable, MobilityWritable<TwoInt>, LongWritable,
            MobilityWritable<Int64>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, MobilityWritable<TwoInt>,
                LongWritable, MobilityWritable<Int64>>(
                        new AdjSpreadTableByPoiIdMapper());
    }
    
    @Test
    public void testMap() {
        this.driver
                .withInput(new LongWritable(57L),
                           new MobilityWritable<TwoInt>(TwoIntUtil.create(3L, 6L)))
                .withOutput(new LongWritable(3L), MobilityWritable.create(6L))
                .runTest();
    }
}
