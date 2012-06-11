package es.tid.cosmos.mobility.populationdensity;

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

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeWithoutProfileReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<BtsProfile>, ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                ProtobufWritable<MobData>, ProtobufWritable<BtsProfile>,
                ProtobufWritable<MobData>>(
                        new PopdenJoinNodeInfoWithoutProfileReducer());
    }

    @Test
    public void testReduce() {
        this.instance
                .withInput(new LongWritable(0L), Arrays.asList(
                                MobDataUtil.createAndWrap(NodeBtsUtil.create(1L,
                                        2L, 3, 4)),
                                MobDataUtil.createAndWrap(NodeBtsUtil.create(5L,
                                        6L, 7, 8))))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 0, 3, 4),
                            MobDataUtil.createAndWrap(1))
                .withOutput(BtsProfileUtil.createAndWrap(6L, 0, 7, 8),
                            MobDataUtil.createAndWrap(1))
                .runTest();
    }
}
