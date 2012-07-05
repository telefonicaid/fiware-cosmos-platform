package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsEntryParser;

/**
 *
 * @author dmicol
 */
public class CellIdAndDateMapper extends Mapper<LongWritable, Text,
                                                BinaryKey, Text> {
    
    private TrafficCountsEntryParser countsParser;
    private BinaryKey cellIdAndDate;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.countsParser = new TrafficCountsEntryParser(
                context.getConfiguration().getStrings(Config.COUNT_FIELDS));
        this.cellIdAndDate = new BinaryKey();
    }
    
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final String valueStr = value.toString();
        final TrafficCountsEntry entry = this.countsParser.parse(valueStr);
        if (entry == null) {
            throw new IllegalArgumentException("Invalid input data: " + valueStr);
        }
        this.cellIdAndDate.setPrimaryKey(entry.cellId);
        this.cellIdAndDate.setSecondaryKey(entry.date);
        context.write(this.cellIdAndDate, value);
    }
}
