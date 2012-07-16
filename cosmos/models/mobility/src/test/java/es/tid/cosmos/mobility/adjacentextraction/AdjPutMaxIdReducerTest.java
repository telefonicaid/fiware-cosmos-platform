package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjPutMaxIdReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<TwoInt>, LongWritable,
            TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<TwoInt>,
                LongWritable, TypedProtobufWritable<TwoInt>>(
                        new AdjPutMaxIdReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<TwoInt> value1 =
                new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(1L, 3L));
        final TypedProtobufWritable<TwoInt> value2 =
                new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(2L, 10L));
        final TypedProtobufWritable<TwoInt> value3 =
                new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(3L, 1L));
        final TypedProtobufWritable<TwoInt> value4 =
                new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(4L, 7L));
        this.driver
                .withInput(key, asList(value1, value2, value3, value4))
                .withOutput(key, new TypedProtobufWritable<TwoInt>(
                        TwoIntUtil.create(4L, 10l)))
                .runTest();
    }
}
