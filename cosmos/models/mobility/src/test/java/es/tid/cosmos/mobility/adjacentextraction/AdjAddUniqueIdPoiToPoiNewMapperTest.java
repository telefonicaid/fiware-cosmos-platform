package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol, sortega
 */
public class AdjAddUniqueIdPoiToPoiNewMapperTest {

    private MapDriver<LongWritable, TypedProtobufWritable<Poi>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<Poi>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>>(
                new AdjAddUniqueIdPoiToPoiNewMapper());
    }

    @Test
    public void testConfidentNodeBts() throws IOException {
        final long uuid = 761478;
        final int confident = 1;
        final int labelGroupNodeBts = 11;
        final Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, confident, 4.3D, 6,
                                       7, 0, 9.1D, 10, labelGroupNodeBts, 1,
                                       8.45D, 1, 0);
        final ProtobufWritable<TwoInt> expectedOutputKey =
                TwoIntUtil.createAndWrap(2L, 11L);
        final PoiNew expectedOutputPoi = PoiNewUtil.create(uuid,
                2L, 3, labelGroupNodeBts, confident);

        this.driver.withInput(new LongWritable(uuid),
                              new TypedProtobufWritable<Poi>(poi))
                   .withOutput(expectedOutputKey,
                               new TypedProtobufWritable<PoiNew>(expectedOutputPoi))
                   .runTest();
    }
}
