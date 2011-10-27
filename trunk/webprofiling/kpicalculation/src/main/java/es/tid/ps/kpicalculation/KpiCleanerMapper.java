package es.tid.ps.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * 
 * This class makes the filtering of data in the cdrs received.
 * Not Implemented yet 
 * @author javierb@tid.es
 */
public class KpiCleanerMapper extends Mapper<LongWritable, Text, LongWritable, Text> {

    

    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        
        context.write(key, value);
    }
}
