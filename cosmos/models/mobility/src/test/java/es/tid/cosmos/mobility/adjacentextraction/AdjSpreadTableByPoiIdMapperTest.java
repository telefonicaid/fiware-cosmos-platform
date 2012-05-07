package es.tid.cosmos.mobility.adjacentextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new AdjSpreadTableByPoiIdMapper());
    }
    
    @Test
    public void testMap() {
        this.driver
                .withInput(new LongWritable(57L),
                           MobDataUtil.createAndWrap(TwoIntUtil.create(3L, 6L)))
                .withOutput(new LongWritable(3L), MobDataUtil.createAndWrap(6L))
                .runTest();
    }
}
