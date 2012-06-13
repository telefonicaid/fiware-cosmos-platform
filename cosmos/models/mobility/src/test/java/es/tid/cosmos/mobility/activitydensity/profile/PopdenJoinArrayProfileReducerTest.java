package es.tid.cosmos.mobility.activitydensity.profile;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class PopdenJoinArrayProfileReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<BtsProfile>, ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                ProtobufWritable<MobData>, ProtobufWritable<BtsProfile>,
                ProtobufWritable<MobData>>(new PopdenJoinArrayProfileReducer());
    }

    @Test
    public void testReduce() {
        final BtsCounter counter1 = BtsCounterUtil.create(10L, 20, 30, 40);
        final BtsCounter counter2 = BtsCounterUtil.create(50L, 60, 70, 80);
        final ProtobufWritable<MobData> counter = MobDataUtil.createAndWrap(
                NodeMxCounterUtil.create(Arrays.asList(counter1, counter2),
                                         3, 4));
        final ProtobufWritable<MobData> profile = MobDataUtil.createAndWrap(
                ClientProfileUtil.create(1L, 5));
        this.instance
                .withInput(new LongWritable(2L),
                           Arrays.asList(counter, profile))
                .withOutput(BtsProfileUtil.createAndWrap(10L, 5, 20, 30),
                            MobDataUtil.createAndWrap(40))
                .withOutput(BtsProfileUtil.createAndWrap(50L, 5, 60, 70),
                            MobDataUtil.createAndWrap(80))
                .runTest();
    }
}
