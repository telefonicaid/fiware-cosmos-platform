package es.tid.cosmos.mobility.populationdensity.profile;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;

/**
 *
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBtsDate>, TypedProtobufWritable<Null>,
            LongWritable, TypedProtobufWritable<NodeBts>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<NodeBtsDate>,
                TypedProtobufWritable<Null>, LongWritable, TypedProtobufWritable<NodeBts>>(
                        new PopdenDeleteDuplicatesReducer());
    }

    @Test
    public void testReduce() {
        ProtobufWritable<NodeBtsDate> key = NodeBtsDateUtil.createAndWrap(
                1L, 2L, DateUtil.create(3, 4, 5, 6), 7);
        this.instance
                .withInput(key, Arrays.asList(
                           new TypedProtobufWritable<Null>(Null.getDefaultInstance())))
                .withOutput(new LongWritable(1L),
                            new TypedProtobufWritable<NodeBts>(
                                    NodeBtsUtil.create(1L, 2L, 6, 7)))
                .runTest();
    }
}
