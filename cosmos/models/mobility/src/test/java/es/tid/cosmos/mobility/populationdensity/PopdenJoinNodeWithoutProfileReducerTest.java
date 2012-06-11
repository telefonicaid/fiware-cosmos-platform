package es.tid.cosmos.mobility.populationdensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeWithoutProfileReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<NodeBts>,
            ProtobufWritable<BtsProfile>, MobilityWritable<Int>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable, MobilityWritable<NodeBts>,
                ProtobufWritable<BtsProfile>, MobilityWritable<Int>>(
                        new PopdenJoinNodeInfoWithoutProfileReducer());
    }

    @Test
    public void testReduce() {
        this.instance
                .withInput(new LongWritable(0L), Arrays.asList(
                                new MobilityWritable<NodeBts>(NodeBtsUtil.create(1L,
                                        2L, 3, 4)),
                                new MobilityWritable<NodeBts>(NodeBtsUtil.create(5L,
                                        6L, 7, 8))))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 0, 3, 4),
                            MobilityWritable.create(1))
                .withOutput(BtsProfileUtil.createAndWrap(6L, 0, 7, 8),
                            MobilityWritable.create(1))
                .runTest();
    }
}
