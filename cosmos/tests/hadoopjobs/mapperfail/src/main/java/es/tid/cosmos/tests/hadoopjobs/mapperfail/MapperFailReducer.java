package es.tid.cosmos.tests.hadoopjobs.mapperfail;

import java.io.IOException;

import org.apache.hadoop.io.ArrayWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author ximo
 */
public class MapperFailReducer
        extends Reducer<Text, IntWritable, Text, ArrayWritable> {
    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        // No body since mapper will always fail
    }
}
