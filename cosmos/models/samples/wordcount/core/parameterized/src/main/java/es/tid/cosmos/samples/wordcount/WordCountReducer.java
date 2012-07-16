package es.tid.cosmos.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * Reducer for word counts
 *
 * @author logc
 */
public class WordCountReducer extends Reducer<Text, IntWritable,
                                              Text, LongWritable> {
    private LongWritable sum;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.sum = new LongWritable();
    }

    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        long sumValue = 0L;
        for (IntWritable val : values) {
            sumValue += val.get();
        }
        this.sum.set(sumValue);
        context.write(key, this.sum);
    }
}
