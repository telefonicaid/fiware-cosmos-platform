package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author dmicol
 */
public class AdjSpreadCountReducer extends Reducer<LongWritable, LongWritable,
        LongWritable, LongWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<LongWritable> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (LongWritable value : values) {
            valueCount++;
        }
        context.write(new LongWritable(0L), new LongWritable(valueCount));
    }
}
