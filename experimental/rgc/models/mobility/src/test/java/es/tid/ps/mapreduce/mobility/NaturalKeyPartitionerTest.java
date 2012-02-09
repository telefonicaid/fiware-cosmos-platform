package es.tid.ps.mapreduce.mobility;

import junit.framework.TestCase;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.Partitioner;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;
import es.tid.ps.mapreduce.mobility.data.FileType;

/**
 * Test case for NaturalKeyPartitioner
 * 
 * @author rgc
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class NaturalKeyPartitionerTest extends TestCase {
    // all the test are over the same users, so the expect partition  is always the same
    private int expected = 2;
    private int numPartitions = 3;
    private String userId = "identifier";
    
    @Test
    public void testUsersFileInput() {
        Partitioner<CompositeKey, Text> partition = new NaturalKeyPartitioner();        
        CompositeKey key = new CompositeKey(userId, FileType.FILE_USERS.getValue());
        Text value = new Text();        
        int out = partition.getPartition(key, value, numPartitions);
        assertEquals(expected, out);
    }
    
    @Test
    public void testUsersFileInputWithValue() {
        Partitioner<CompositeKey, Text> partition = new NaturalKeyPartitioner();        
        CompositeKey key = new CompositeKey(userId, FileType.FILE_USERS.getValue());
        Text value = new Text("value");        
        int out = partition.getPartition(key, value, numPartitions);
        assertEquals(expected, out);
    }
    
    @Test
    public void testCdrsFileInput() {
        Partitioner<CompositeKey, Text> partition = new NaturalKeyPartitioner();        
        CompositeKey key = new CompositeKey(userId, FileType.FILE_CDRS.getValue());
        Text value = new Text();        
        int out = partition.getPartition(key, value, numPartitions);
        assertEquals(expected, out);
    }
    
    @Test
    public void testCdrsFileInputWithValue() {
        Partitioner<CompositeKey, Text> partition = new NaturalKeyPartitioner();        
        CompositeKey key = new CompositeKey(userId, FileType.FILE_CDRS.getValue());
        Text value = new Text("value");        
        int out = partition.getPartition(key, value, numPartitions);
        assertEquals(expected, out);
    }    
}