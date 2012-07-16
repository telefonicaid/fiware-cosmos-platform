package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadCountReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<TwoInt>,
            LongWritable, TypedProtobufWritable<Int64>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<TwoInt>,
                LongWritable, TypedProtobufWritable<Int64>>(
                        new AdjSpreadCountReducer());
    }

    @Test
    public void testReduce() {
        final TypedProtobufWritable<TwoInt> value1 = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(100L, 200L));
        final TypedProtobufWritable<TwoInt> value2 = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(300L, 400L));
        this.driver
                .withInput(new LongWritable(57L), asList(value1, value2))
                .withOutput(new LongWritable(0L), TypedProtobufWritable.create(2L))
                .runTest();
    }
}
