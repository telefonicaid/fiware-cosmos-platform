package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author dmicol
 */
public class AdjCountIndexesReducer extends Reducer <LongWritable, LongWritable,
        LongWritable, NullWritable> {
    public static final String NUM_INDICES_LEFT_TAG = "num_indices_left";
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<LongWritable> values, Context context)
            throws IOException, InterruptedException {
        long sum = 0;
        for (LongWritable value : values) {
            sum += value.get();
        }
        context.getConfiguration().setLong(NUM_INDICES_LEFT_TAG, sum);
        context.write(new LongWritable(sum), NullWritable.get());
    }
}
