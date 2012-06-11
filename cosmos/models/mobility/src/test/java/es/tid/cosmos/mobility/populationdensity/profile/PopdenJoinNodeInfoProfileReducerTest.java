package es.tid.cosmos.mobility.populationdensity.profile;

import java.util.Arrays;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoProfileReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>,
            ProtobufWritable<BtsProfile>, MobilityWritable<Int64>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                ProtobufWritable<BtsProfile>, MobilityWritable<Int64>>(
                        new PopdenJoinNodeInfoProfileReducer());
    }

    @Test
    public void testReduce() {
        MobilityWritable<Message> nodebts = new MobilityWritable<Message>(
                NodeBtsUtil.create(1L, 2L, 3, 4));
        MobilityWritable<Message> intdata = new MobilityWritable<Message>(
                ClientProfileUtil.create(1L, 5));
        this.instance
                .withInput(new LongWritable(2L),
                           Arrays.asList(nodebts, intdata))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 5, 3, 4),
                            MobilityWritable.create(1L))
                .runTest();
    }
}
