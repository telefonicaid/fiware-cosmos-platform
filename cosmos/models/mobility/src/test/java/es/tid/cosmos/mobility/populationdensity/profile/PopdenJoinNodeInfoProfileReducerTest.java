package es.tid.cosmos.mobility.populationdensity.profile;

import java.util.Arrays;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoProfileReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
            ProtobufWritable<BtsProfile>, TypedProtobufWritable<Int>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                ProtobufWritable<BtsProfile>, TypedProtobufWritable<Int>>(
                        new PopdenJoinNodeInfoProfileReducer());
    }

    @Test
    public void testReduce() {
        TypedProtobufWritable<Message> nodebts = new TypedProtobufWritable<Message>(
                NodeBtsUtil.create(1L, 2L, 3, 4));
        TypedProtobufWritable<Message> intdata = new TypedProtobufWritable<Message>(
                ClientProfileUtil.create(1L, 5));
        this.instance
                .withInput(new LongWritable(2L),
                           Arrays.asList(nodebts, intdata))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 5, 3, 4),
                            TypedProtobufWritable.create(1))
                .runTest();
    }
}
