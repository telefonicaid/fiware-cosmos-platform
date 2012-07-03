package es.tid.cosmos.mobility.adjacentextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author sortega
 */
public class AdjAddUUIDToPoiMapperTest {

    private MapDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
                      LongWritable, TypedProtobufWritable<Poi>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<TwoInt>,
            TypedProtobufWritable<Poi>, LongWritable,
            TypedProtobufWritable<Poi>>(new AdjAddUUIDToPoiMapper());
    }

    @Test
    public void testReduce() throws Exception {
        final ProtobufWritable<TwoInt> k1 = TwoIntUtil.createAndWrap(57L, 88L),
                                       k2 = TwoIntUtil.createAndWrap(10L, 20L);
        final Poi poi1 = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7, 0,
                                        9.1D, 10, 11, 1, 8.45D, 1, 0),
                  poi2 = PoiUtil.create(9, 4L, 2, 4, 6, 8, 2.7d, 10, 12, 14,
                                        9.1d, 10, 11, 1, 8.45d, 1, 0);

        Pair<LongWritable, TypedProtobufWritable<Poi>> firstResult =
                this.driver.withInput(k1, new TypedProtobufWritable<Poi>(poi1))
                           .run().get(0);
        assertEquals(poi1, firstResult.getSecond().get());

        Pair<LongWritable, TypedProtobufWritable<Poi>> secondResult =
                this.driver.withInput(k2, new TypedProtobufWritable<Poi>(poi2))
                           .run().get(0);
        assertEquals(poi2, secondResult.getSecond().get());

        long newKey1 = firstResult.getFirst().get();
        long newKey2 = secondResult.getFirst().get();
        assertFalse(newKey1 == newKey2);
    }
}
