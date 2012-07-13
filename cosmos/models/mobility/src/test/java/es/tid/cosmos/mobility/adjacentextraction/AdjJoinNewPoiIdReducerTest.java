package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjJoinNewPoiIdReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>>(
                        new AdjJoinNewPoiIdReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(103L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(57L).build());
        final TypedProtobufWritable<Message> value2 =
                new TypedProtobufWritable<Message>(PoiNewUtil.create(1, 2, 3, 4, 0));
        final TypedProtobufWritable<Message> value3 = new TypedProtobufWritable<Message>(
                Int64.newBuilder().setValue(32L).build());
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final TypedProtobufWritable<PoiNew> outValue1 = new TypedProtobufWritable<PoiNew>(
                PoiNewUtil.create(57, 2, 3, 4, 0));
        final TypedProtobufWritable<PoiNew> outValue2 = new TypedProtobufWritable<PoiNew>(
                PoiNewUtil.create(32, 2, 3, 4, 0));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue1)
                .withOutput(outKey, outValue2)
                .runTest();
    }
}
