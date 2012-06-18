package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;
import es.tid.smartsteps.dispersion.parsing.Parser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsEntryParser;

/**
 *
 * @author dmicol
 */
public class DateFilterMapper extends Mapper<LongWritable, Text,
                                             LongWritable, Text> {
    private String dateToFilter;
    private Parser trafficCountsEntryPaser;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.dateToFilter = context.getConfiguration().get(
                Config.DATE_TO_FILTER);
        this.trafficCountsEntryPaser = new TrafficCountsEntryParser();
    }
    
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        TrafficCountsEntry entry =
                (TrafficCountsEntry) this.trafficCountsEntryPaser
                        .parse(value.toString());
        if (!entry.date.equals(this.dateToFilter)) {
            return;
        }
        context.write(key, value);
    }
}
