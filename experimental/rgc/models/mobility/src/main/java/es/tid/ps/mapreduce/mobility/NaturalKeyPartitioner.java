package es.tid.ps.mapreduce.mobility;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.Partitioner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;

/**
 * NaturalKeyPartitioner
 * 
 * Purpose: partitions the data output from the map phase before it is sent
 * through the shuffle phase.
 * 
 * The main method to pay attention to in this example is called
 * "getPartition()"
 * 
 * getPartition() determines how we group the data; In the case of this
 * secondary sort example this function partitions the data by only the first
 * half of the key
 * 
 * @author rgc
 */
public class NaturalKeyPartitioner implements Partitioner<CompositeKey, Text> {

    /**
     * Get the paritition number for a given key, in this case the partition is
     * calculate from the naturalKey of the structure CompositeKey
     * 
     * @param key
     *            the key to be paritioned.
     * @param value
     *            the entry value.
     * @param numPartitions
     *            the total number of partitions.
     * @return the partition number for the <code>key</code>.
     */
    @Override
    public int getPartition(CompositeKey key, Text value, int numPartitions) {
        return Math.abs(key.getUserIdKey().hashCode() * 127) % numPartitions;
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * org.apache.hadoop.mapred.JobConfigurable#configure(org.apache.hadoop.
     * mapred.JobConf)
     */
    @Override
    public void configure(JobConf job) {
    }
}
