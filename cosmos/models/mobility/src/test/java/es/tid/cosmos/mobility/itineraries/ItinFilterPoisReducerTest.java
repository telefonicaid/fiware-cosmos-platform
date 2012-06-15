package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.Arrays;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ItinFilterPoisReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>,
            TypedProtobufWritable<Message>, LongWritable,
            TypedProtobufWritable<ItinTime>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, LongWritable,
                TypedProtobufWritable<ItinTime>>(new ItinFilterPoisReducer());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 88L);
        final TypedProtobufWritable<Message> value1 =
                new TypedProtobufWritable<Message>(ItinTimeUtil.create(
                        Date.getDefaultInstance(), Time.getDefaultInstance(), 3L));
        final TypedProtobufWritable<Message> value2 =
                new TypedProtobufWritable<Message>(PoiUtil.create(1, 2, 3, 4, 5,
                        6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17));
        final LongWritable outKey = new LongWritable(57L);
        final TypedProtobufWritable<ItinTime> outValue =
                new TypedProtobufWritable<ItinTime>(ItinTimeUtil.create(
                        Date.getDefaultInstance(), Time.getDefaultInstance(), 1L));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(outKey, outValue)
                .run();
    }
}
