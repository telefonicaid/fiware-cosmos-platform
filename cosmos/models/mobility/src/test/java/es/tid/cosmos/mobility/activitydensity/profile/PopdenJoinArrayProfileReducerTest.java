package es.tid.cosmos.mobility.activitydensity.profile;

import java.util.Arrays;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;

/**
 *
 * @author dmicol
 */
public class PopdenJoinArrayProfileReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>,
            ProtobufWritable<BtsProfile>, MobilityWritable<Int>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                MobilityWritable<Message>, ProtobufWritable<BtsProfile>,
                MobilityWritable<Int>>(new PopdenJoinArrayProfileReducer());
    }

    @Test
    public void testReduce() {
        final BtsCounter counter1 = BtsCounterUtil.create(10L, 20, 30, 40);
        final BtsCounter counter2 = BtsCounterUtil.create(50L, 60, 70, 80);
        final MobilityWritable<Message> counter = new MobilityWritable<Message>(
                NodeMxCounterUtil.create(Arrays.asList(counter1, counter2),
                                         3, 4));
        final MobilityWritable<Message> profile = new MobilityWritable<Message>(
                ClientProfileUtil.create(1L, 5));
        this.instance
                .withInput(new LongWritable(2L),
                           Arrays.asList(counter, profile))
                .withOutput(BtsProfileUtil.createAndWrap(10L, 5, 20, 30),
                            MobilityWritable.create(40))
                .withOutput(BtsProfileUtil.createAndWrap(50L, 5, 60, 70),
                            MobilityWritable.create(80))
                .runTest();
    }
}
