package es.tid.bdp.kpicalculation;

import java.io.IOException;
import static java.util.Arrays.asList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.SingleKey;

/**
 *
 * @author dmicol
 */
public class KpiCounterCombinerTest {
    private KpiCounterCombiner instance;
    private ReduceDriver<SingleKey, IntWritable, SingleKey, IntWritable> driver;

    @Before
    public void setUp() {
        this.instance = new KpiCounterCombiner();
        this.driver = new ReduceDriver<SingleKey, IntWritable,
                                       SingleKey, IntWritable>(this.instance);
    }
    
    @Test
    public void shouldCountPairs() throws IOException {
        SingleKey key = new SingleKey("a");
        this.driver
                .withInput(key, asList(new IntWritable(2), new IntWritable(3)))
                .withOutput(key, new IntWritable(5))
                .run();
    }
}
