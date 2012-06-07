package es.tid.cosmos.mobility.populationdensity.profile;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.ClientProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoProfileReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<BtsProfile>, ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<BtsProfile>, ProtobufWritable<MobData>>(
                        new PopdenJoinNodeInfoProfileReducer());
    }

    @Test
    public void testReduce() {
        ProtobufWritable<MobData> nodebts = MobDataUtil.createAndWrap(
                NodeBtsUtil.create(1L, 2L, 3, 4));
        ProtobufWritable<MobData> intdata = MobDataUtil.createAndWrap(
                ClientProfileUtil.create(1L, 5));
        this.instance
                .withInput(new LongWritable(2L),
                           Arrays.asList(nodebts, intdata))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 5, 3, 4),
                            MobDataUtil.createAndWrap(1))
                .runTest();
    }
}
