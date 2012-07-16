package es.tid.cosmos.kpicalculation;

import java.io.IOException;
import static java.util.Arrays.asList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 *
 * @author dmicol
 */
public class KpiCounterCombinerTest {
    private KpiCounterCombiner instance;
    private ReduceDriver<CompositeKey, IntWritable, CompositeKey, IntWritable>
            driver;

    @Before
    public void setUp() {
        this.instance = new KpiCounterCombiner();
        this.driver = new ReduceDriver<CompositeKey, IntWritable, CompositeKey,
                IntWritable>(this.instance);
    }

    @Test
    public void shouldCountPairs() throws IOException {
        CompositeKey key = new CompositeKey(1);
        key.set(0, "a");
        this.driver
                .withInput(key, asList(new IntWritable(2), new IntWritable(3)))
                .withOutput(key, new IntWritable(5))
                .run();
    }
}
