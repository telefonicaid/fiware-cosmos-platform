package es.tid.cosmos.mobility.propulationdensity.profile;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenJoinNodeInfoProfileReducer;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoProfileReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<BtsProfile>, LongWritable> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<BtsProfile>, LongWritable>(
                new PopdenJoinNodeInfoProfileReducer());
    }

    @Test
    public void testReduce() {
        ProtobufWritable<MobData> nodebts = MobDataUtil.createAndWrap(
                NodeBtsUtil.create(1L, 2L, 3, 4));
        ProtobufWritable<MobData> intdata = MobDataUtil.createAndWrap(5);
        this.driver
                .withInput(new LongWritable(2L),
                           Arrays.asList(nodebts, intdata))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 5, 3, 4),
                            new LongWritable(1L))
                .runTest();
    }
}
