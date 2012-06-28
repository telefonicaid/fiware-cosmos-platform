package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.smartsteps.dispersion.data.Entry;
import es.tid.smartsteps.dispersion.parsing.CellToMicrogridEntryParser;
import es.tid.smartsteps.dispersion.parsing.MicrogridToPolygonEntryParser;
import es.tid.smartsteps.dispersion.parsing.Parser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsEntryParser;

/**
 *
 * @author dmicol
 */
public class EntryScalerMapper extends Mapper<LongWritable, Text,
                                              Text, Text> {

    private Parser countsParser;
    private Parser lookupParser;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.countsParser = new TrafficCountsEntryParser(
                context.getConfiguration().getStrings(Config.COUNT_FIELDS));
        LookupType type = context.getConfiguration().getEnum(
                LookupType.class.getName(), LookupType.INVALID);
        switch (type) {
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
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        String cellId;
        Entry entry;
        final String valueStr = value.toString();
        if ((entry = this.countsParser.parse(valueStr)) != null || 
                (entry = this.lookupParser.parse(valueStr)) != null) {
            cellId = entry.getKey();
        } else {
            throw new IllegalArgumentException("Invalid input data: " + valueStr);
        }
        context.write(new Text(cellId), value);
    }
}
