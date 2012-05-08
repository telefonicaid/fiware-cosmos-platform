package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntNewMapperTest {
    private MapDriver<
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, LongWritable,
                ProtobufWritable<MobData>>(
                        new AdjAddUniqueIdPoiToTwoIntMapper());
    }
    
    @Test
    public void testMap() throws IOException {
        Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                 0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<LongWritable, ProtobufWritable<MobData>>> result =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(137L, 201L),
                                   MobDataUtil.createAndWrap(poi))
                        .run();
        final LongWritable key = result.get(0).getFirst();
        assertEquals(37L, key.get());
        ProtobufWritable<MobData> valueWrapper = result.get(0).getSecond();
        valueWrapper.setConverter(MobData.class);
        final TwoInt value = valueWrapper.get().getTwoInt();
        assertEquals(37L, value.getNum1());
        assertEquals(37L, value.getNum2());
    }
}
