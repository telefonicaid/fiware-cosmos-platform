package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToPoiNewMapperTest {
    private MapDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>, ProtobufWritable<TwoInt>, 
                TypedProtobufWritable<PoiNew>>(
                        new AdjAddUniqueIdPoiToPoiNewMapper());
    }
    
    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 88L);
        final Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                       0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>>> res =
                this.driver
                        .withInput(key, new TypedProtobufWritable<Poi>(poi))
                        .run();
        assertEquals(1, res.size());
        ProtobufWritable<TwoInt> keyWrapper = res.get(0).getFirst();
        keyWrapper.setConverter(TwoInt.class);
        final TwoInt outKey = keyWrapper.get();
        assertEquals(2L, outKey.getNum1());
        assertEquals(11L, outKey.getNum2());
        TypedProtobufWritable<PoiNew> valueWrapper = res.get(0).getSecond();
        PoiNew value = valueWrapper.get();
        assertEquals(0, value.getId());
    }
}
