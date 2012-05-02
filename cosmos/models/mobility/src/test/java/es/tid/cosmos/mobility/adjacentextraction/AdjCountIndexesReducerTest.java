package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class AdjCountIndexesReducerTest {
    private ReduceDriver<LongWritable, LongWritable, LongWritable, NullWritable>
            driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, LongWritable, LongWritable,
                NullWritable>(new AdjCountIndexesReducer());
    }
    
    @Test
    public void testReduce() {
        this.driver
                .withInput(new LongWritable(3L),
                           asList(new LongWritable(5L), new LongWritable(10L),
                                  new LongWritable(7L), new LongWritable(0L)))
                .withOutput(new LongWritable(22L), NullWritable.get())
                .runTest();
    }
}
