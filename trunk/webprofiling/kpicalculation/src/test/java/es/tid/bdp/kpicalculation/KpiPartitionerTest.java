package es.tid.bdp.kpicalculation;

import org.apache.hadoop.io.IntWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.base.mapreduce.BinaryKey;

/**
 *
 * @author dmicol
 */
public class KpiPartitionerTest {
    private KpiPartitioner instance;
    
    @Before
    public void setUp() {
        this.instance = new KpiPartitioner();
    }
    
    @Test
    public void testGetPartition() {
        assertEquals(6, this.instance.getPartition(new BinaryKey("a", "b"), 
                                                   new IntWritable(3), 7));
    }
}
