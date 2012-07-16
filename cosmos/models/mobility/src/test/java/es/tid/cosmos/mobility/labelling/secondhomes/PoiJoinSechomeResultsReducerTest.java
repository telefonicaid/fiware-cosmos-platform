package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
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
public class PoiJoinSechomeResultsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>>(new PoiJoinSechomeResultsReducer());
    }

    @Test
    public void testOutputWithSecHomeCount() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                TwoIntUtil.create(52L, 37L));
        final TypedProtobufWritable<Message> value3 = new TypedProtobufWritable<Message>(
                Null.getDefaultInstance());
        final TypedProtobufWritable<Poi> outValue = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 100, 100, 14,
                               15, 52, 37));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testOutputWithoutSecHomeCount() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                TwoIntUtil.create(52L, 37L));
        final TypedProtobufWritable<Poi> outValue = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 52, 37));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
