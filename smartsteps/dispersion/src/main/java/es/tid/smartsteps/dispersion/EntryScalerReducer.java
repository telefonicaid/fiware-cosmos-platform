package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.smartsteps.dispersion.data.Entry;
import es.tid.smartsteps.dispersion.data.LookupEntry;
import es.tid.smartsteps.dispersion.data.LookupTable;
import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;
import es.tid.smartsteps.dispersion.parsing.CellToMicrogridEntryParser;
import es.tid.smartsteps.dispersion.parsing.MicrogridToPolygonEntryParser;
import es.tid.smartsteps.dispersion.parsing.Parser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsEntryParser;

/**
 *
 * @author dmicol
 */
public class EntryScalerReducer extends Reducer<Text, Text,
                                                NullWritable, Text> {

    private LookupType type;
    private Parser countsParser;
    private Parser lookupParser;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.type = context.getConfiguration().getEnum(
                LookupType.class.getName(), LookupType.INVALID);
        this.countsParser = new TrafficCountsEntryParser(
                context.getConfiguration().getStrings(Config.COUNT_FIELDS));
        switch (this.type) {
            case CELL_TO_MICROGRID:
                this.lookupParser = new CellToMicrogridEntryParser(
                        context.getConfiguration().get(Config.DELIMITER));
                break;
            case MICROGRID_TO_POLYGON:
                this.lookupParser = new MicrogridToPolygonEntryParser(
                        context.getConfiguration().get(Config.DELIMITER));
                break;
            default:
                throw new IllegalArgumentException("Invalid lookup type");
        }
    }
 
    @Override
    protected void reduce(Text key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        List<TrafficCountsEntry> trafficCountsEntries =
                new LinkedList<TrafficCountsEntry>();
        LookupTable lookupTable = new LookupTable();
        for (Text value : values) {
            Entry entry;
            if ((entry = this.countsParser.parse(value.toString())) != null) {
                trafficCountsEntries.add((TrafficCountsEntry) entry);
            } else if ((entry = this.lookupParser.parse(value.toString()))
                    != null) {
                lookupTable.add(entry);
            } else {
                throw new IllegalArgumentException("Invalid input data");
            }
        }
        
        for (TrafficCountsEntry entry : trafficCountsEntries) {
            final List<LookupEntry> lookups = lookupTable.get(key.toString());
            if (lookups == null) {
                context.getCounter(Counters.ENTRIES_NOT_IN_LOOKUP).increment(1L);
                continue;
            }
            context.getCounter(Counters.ENTRIES_IN_LOOKUP).increment(1L);
            for (LookupEntry lookup : lookups) {
                TrafficCountsEntry scaledEntry = entry.scale(
                        lookup.getProportion());
                scaledEntry.cellId = lookup.getSecondaryKey();
                context.write(NullWritable.get(),
                              new Text(scaledEntry.toJSON().toString()));
            }
        }
    }
}
