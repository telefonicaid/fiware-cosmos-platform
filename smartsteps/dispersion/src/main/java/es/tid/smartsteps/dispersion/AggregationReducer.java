package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsEntryParser;

/**
 *
 * @author dmicol
 */
public class AggregationReducer extends Reducer<BinaryKey, Text,
                                                NullWritable, Text> {
    
    private TrafficCountsEntryParser countsParser;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.countsParser = new TrafficCountsEntryParser(
                context.getConfiguration().getStrings(Config.COUNT_FIELDS));
    }
    
    @Override
    protected void reduce(BinaryKey key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        TrafficCountsEntry aggregatedEntry = null;
        for (Text value : values) {
            final String valueStr = value.toString();
            final TrafficCountsEntry entry = this.countsParser.parse(valueStr);
            if (entry == null) {
                throw new IllegalArgumentException(
                        "Invalid input data: " + valueStr);
            }
            if (aggregatedEntry == null) {
                aggregatedEntry = new TrafficCountsEntry(entry);
            } else {
                aggregatedEntry = aggregatedEntry.aggregate(entry);
            }
        }
        context.write(NullWritable.get(),
                      new Text(aggregatedEntry.toJSON().toString()));
    }
}
