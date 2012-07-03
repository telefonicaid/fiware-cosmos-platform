package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol, sortega
 */
public class AdjAddUniqueIdPoiToTwoIntMapperTest {

    private MapDriver<LongWritable, TypedProtobufWritable<Poi>,
                      LongWritable, TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<Poi>,
                LongWritable, TypedProtobufWritable<TwoInt>>(
                new AdjAddUniqueIdPoiToTwoIntMapper());
    }

    @Test
    public void testReduce() throws IOException {
        final LongWritable uuid = new LongWritable(761478L);
        final Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                       0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        this.driver.withInput(uuid, new TypedProtobufWritable<Poi>(poi))
                .withOutput(uuid, new TypedProtobufWritable<TwoInt>(
                    TwoIntUtil.create(761478L, 761478L)
                ))
                .runTest();
    }
}
