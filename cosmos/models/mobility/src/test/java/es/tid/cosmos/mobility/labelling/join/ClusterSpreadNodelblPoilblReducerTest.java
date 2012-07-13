package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterSpreadNodelblPoilblReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Null>>(
                        new ClusterSpreadNodelblPoilblReducer());
    }
    
    @Test
    public void testTwoOutputs() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Poi> value1 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 1, 6, 7, 8, 9, 10, 11, 12, 1, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey1 = TwoIntUtil.createAndWrap(4,
                                                                          11);
        final TypedProtobufWritable<Poi> value2 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(10, 20, 30, 40, 50, 1, 60, 70, 80, 90, 100, 110,
                               120, 1, 130, 140, 150));
        final ProtobufWritable<TwoInt> outKey2 = TwoIntUtil.createAndWrap(40,
                                                                          110);
        final TypedProtobufWritable<Null> outValue = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey1, outValue)
                .withOutput(outKey2, outValue)
                .runTest();
    }

    @Test
    public void testOneOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Poi> value1 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 1, 6, 7, 8, 9, 10, 11, 12, 0, 13,
                               14, 15));
        final TypedProtobufWritable<Poi> value2 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(10, 20, 30, 40, 50, 1, 60, 70, 80, 90, 100, 110,
                               120, 1, 130, 140, 150));
        final TypedProtobufWritable<Poi> value3 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 9, 10, 11, 12, 1, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(40,
                                                                         110);
        final TypedProtobufWritable<Null> outValue = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue)
                .runTest();
    }
    
    @Test
    public void testNoOutputs() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Poi> value1 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 9, 10, 11, 12, 0, 13,
                               14, 15));
        final TypedProtobufWritable<Poi> value2 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(10, 20, 30, 40, 50, 0, 60, 70, 80, 90, 100, 110,
                               120, 0, 130, 140, 150));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }
}
