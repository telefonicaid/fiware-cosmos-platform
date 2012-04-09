package es.tid.cosmos.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * 
 * @author javierb
 */
public class KpiCleanerReducer extends
        Reducer<LongWritable, Text, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key, Iterable<Text> values,
            Context context) throws IOException, InterruptedException {
        context.write(NullWritable.get(), values.iterator().next());
    }
}
