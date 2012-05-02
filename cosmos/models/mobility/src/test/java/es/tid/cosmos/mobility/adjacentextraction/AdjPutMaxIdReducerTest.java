package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjPutMaxIdReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<TwoInt>, LongWritable,
            ProtobufWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<TwoInt>,
                LongWritable, ProtobufWritable<TwoInt>>(
                        new AdjPutMaxIdReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<TwoInt> value1 = 
                TwoIntUtil.createAndWrap(1L, 3L);
        final ProtobufWritable<TwoInt> value2 =
                TwoIntUtil.createAndWrap(2L, 10L);
        final ProtobufWritable<TwoInt> value3 =
                TwoIntUtil.createAndWrap(3L, 1L);
        final ProtobufWritable<TwoInt> value4 =
                TwoIntUtil.createAndWrap(4L, 7L);
        this.driver
                .withInput(key, asList(value1, value2, value3, value4))
                .withOutput(key, TwoIntUtil.createAndWrap(4L, 10l))
                .runTest();
    }
}
