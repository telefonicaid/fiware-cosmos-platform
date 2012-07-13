package es.tid.cosmos.tests.hadoopjobs.mapperfail;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 *
 * @author ximo
 */
public class MapperFailMapper
        extends Mapper<LongWritable, Text, Text, IntWritable> {    
    @Override
    public void map(LongWritable keyin, Text value, Context context)
            throws IOException, InterruptedException {
        throw new IOException("This is a test designed to fail!");
    }
}
