package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToPoiNewMapperTest {
    private MapDriver<
            ProtobufWritable<TwoInt>, ProtobufWritable<Poi>,
            ProtobufWritable<TwoInt>, ProtobufWritable<PoiNew>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<Poi>, ProtobufWritable<TwoInt>,
                ProtobufWritable<PoiNew>>(
                        new AdjAddUniqueIdPoiToPoiNewMapper());
    }
    
    @Test
    public void testMap() throws IOException {
        Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                 0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<PoiNew>>> result =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(137L, 201L),
                                   PoiUtil.wrap(poi))
                        .run();
        ProtobufWritable<TwoInt> keyWrapper = result.get(0).getFirst();
        keyWrapper.setConverter(TwoInt.class);
        TwoInt key = keyWrapper.get();
        assertEquals(2, key.getNum1());
        assertEquals(11, key.getNum2());
        ProtobufWritable<PoiNew> valueWrapper = result.get(0).getSecond();
        valueWrapper.setConverter(PoiNew.class);
        PoiNew value = valueWrapper.get();
        assertEquals(37, value.getId());
    }
}
