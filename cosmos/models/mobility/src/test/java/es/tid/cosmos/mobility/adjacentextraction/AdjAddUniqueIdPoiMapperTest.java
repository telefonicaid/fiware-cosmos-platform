package es.tid.cosmos.mobility.adjacentextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiMapperTest {

    private MapDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
            LongWritable, TypedProtobufWritable<Poi>> instance;
    
    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>, LongWritable,
                TypedProtobufWritable<Poi>>(new AdjAddUniqueIdPoiMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 88L);
        final TypedProtobufWritable<Poi> value = new TypedProtobufWritable<Poi>(
                Poi.getDefaultInstance());
        final LongWritable outKey = new LongWritable(57L);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}
