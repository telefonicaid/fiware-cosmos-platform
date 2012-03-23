package es.tid.bdp.kpicalculation;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.BinaryKey;
import java.io.IOException;

/**
 *
 * @author dmicol
 */
public class KpiCounterByCombinerTest {
    private KpiCounterByCombiner instance;
    private ReduceDriver<BinaryKey, IntWritable, BinaryKey, IntWritable> driver;
    
    @Before
    public void setUp() {
        this.instance = new KpiCounterByCombiner();
        this.driver = new ReduceDriver<BinaryKey, IntWritable,
                                       BinaryKey, IntWritable>(this.instance);
    }
    
    @Test
    public void testReduce() throws IOException {
        BinaryKey key = new BinaryKey("a", "b");
        this.driver
                .withInput(key, asList(new IntWritable(2), new IntWritable(3)))
                .withOutput(key, new IntWritable(1))
                .run();
    }
}
