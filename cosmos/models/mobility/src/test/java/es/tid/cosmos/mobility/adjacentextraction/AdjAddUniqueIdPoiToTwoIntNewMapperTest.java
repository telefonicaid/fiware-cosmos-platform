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

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntNewMapperTest {

    private MapDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
            LongWritable, TypedProtobufWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>, LongWritable,
                TypedProtobufWritable<TwoInt>>(
                        new AdjAddUniqueIdPoiToTwoIntMapper());
    }
    
    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 88L);
        final Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                       0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<LongWritable, TypedProtobufWritable<TwoInt>>> res =
                this.driver
                        .withInput(key, new TypedProtobufWritable<Poi>(poi))
                        .run();
        assertEquals(1, res.size());
        final long outKey = res.get(0).getFirst().get();
        assertEquals(0L, outKey);
        TypedProtobufWritable<TwoInt> valueWrapper = res.get(0).getSecond();
        final TwoInt value = valueWrapper.get();
        assertEquals(0L, value.getNum1());
        assertEquals(0L, value.getNum2());
    }
}
