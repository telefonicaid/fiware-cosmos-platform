package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterGetMajPoiByNodeReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<TwoInt>, LongWritable,
            TypedProtobufWritable<Int64>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<TwoInt>,
                LongWritable, TypedProtobufWritable<Int64>>(
                        new ClusterGetMajPoiByNodeReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<TwoInt> value1 = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(34L, 76L));
        final TypedProtobufWritable<TwoInt> value2 = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(52L, 101L));
        final TypedProtobufWritable<TwoInt> value3 = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(107L, 65));
        final TypedProtobufWritable<Int64> outValue = TypedProtobufWritable.create(52L);
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }
}
