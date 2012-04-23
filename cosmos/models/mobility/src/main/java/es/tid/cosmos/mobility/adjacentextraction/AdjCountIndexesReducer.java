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
    @Override
    protected void reduce(LongWritable key,
            Iterable<LongWritable> values, Context context)
            throws IOException, InterruptedException {
        long sum = 0;
        for (LongWritable value : values) {
            sum += value.get();
        }
        context.getConfiguration().setLong("num_indices_left", sum);
        context.write(new LongWritable(sum), NullWritable.get());
    }
}
