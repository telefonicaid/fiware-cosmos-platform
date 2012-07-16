package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
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
public class ClusterAggPotPoiPoisToPoiReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
            LongWritable, TypedProtobufWritable<Poi>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, LongWritable,
                TypedProtobufWritable<Poi>>(
                        new ClusterAggPotPoiPoisToPoiReducer());
    }

    @Test
    public void shouldChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                PoiUtil.create(10, 20, 30, 40, 50, 0, 60, 70, 80, 0, 90, 100,
                               110, 0, 130, 140, 150));
        // This value is what makes the reducer change the confidence
        final TypedProtobufWritable<Message> value3 = new TypedProtobufWritable<Message>(
                Null.getDefaultInstance());
        final LongWritable outKey = new LongWritable(3L);
        final TypedProtobufWritable<Poi> outValue = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 1, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void shouldNotChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                PoiUtil.create(10, 20, 30, 40, 50, 0, 60, 70, 80, 0, 90, 100,
                               110, 0, 130, 140, 150));
        final LongWritable outKey = new LongWritable(3L);
        final TypedProtobufWritable<Poi> outValue = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
