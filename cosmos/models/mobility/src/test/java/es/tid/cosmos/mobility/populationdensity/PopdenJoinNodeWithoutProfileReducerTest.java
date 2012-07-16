package es.tid.cosmos.mobility.populationdensity;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeWithoutProfileReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<NodeBts>,
            ProtobufWritable<BtsProfile>, TypedProtobufWritable<Int>> instance;

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable, TypedProtobufWritable<NodeBts>,
                ProtobufWritable<BtsProfile>, TypedProtobufWritable<Int>>(
                        new PopdenJoinNodeInfoWithoutProfileReducer());
    }

    @Test
    public void testReduce() {
        this.instance
                .withInput(new LongWritable(0L), Arrays.asList(
                                new TypedProtobufWritable<NodeBts>(NodeBtsUtil.create(1L,
                                        2L, 3, 4)),
                                new TypedProtobufWritable<NodeBts>(NodeBtsUtil.create(5L,
                                        6L, 7, 8))))
                .withOutput(BtsProfileUtil.createAndWrap(2L, 0, 3, 4),
                            TypedProtobufWritable.create(1))
                .withOutput(BtsProfileUtil.createAndWrap(6L, 0, 7, 8),
                            TypedProtobufWritable.create(1))
                .runTest();
    }
}
