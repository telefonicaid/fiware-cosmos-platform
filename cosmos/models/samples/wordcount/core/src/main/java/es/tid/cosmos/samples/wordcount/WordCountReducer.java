package es.tid.cosmos.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * Reducer for word counts
 *
 * @author logc
 */
public class WordCountReducer
        extends Reducer<Text, IntWritable, Text, LongWritable> {
    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context) 
          throws IOException, InterruptedException {
        long sum = 0L;
        for (IntWritable val : values) {
            sum += val.get();
        }
        context.write(key, new LongWritable(sum));
    }
}
