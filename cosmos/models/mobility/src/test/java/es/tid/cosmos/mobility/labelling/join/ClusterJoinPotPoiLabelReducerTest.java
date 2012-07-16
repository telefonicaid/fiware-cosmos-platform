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
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterJoinPotPoiLabelReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>>(
                        new ClusterJoinPotPoiLabelReducer());
    }

    @Test
    public void shouldProduceOneOutput() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(10L).build());
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final TypedProtobufWritable<Message> value3 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(32L).build());
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final TypedProtobufWritable<Null> outValue = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void shouldProduceTwoOutputs() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(10L).build());
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final TypedProtobufWritable<Null> outValue = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(key, asList(value1, value2, value1))
                .withOutput(outKey, outValue)
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputs() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(55L).build());
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final TypedProtobufWritable<Message> value3 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(32L).build());
        this.driver
                .withInput(key, asList(value1, value2, value1))
                .runTest();
    }
}
