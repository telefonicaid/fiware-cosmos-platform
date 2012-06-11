package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;

/**
 * Input: <Long, Itinerary>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class ItinItineraryOutReducer extends Reducer<LongWritable,
        MobilityWritable<Itinerary>, NullWritable, Text> {
    private static final String DELIMITER = "|";
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Itinerary>> values, Context context)
            throws IOException, InterruptedException {
        final long node = key.get();
        for (MobilityWritable<Itinerary> value : values) {
            final Itinerary itin = value.get();
            String output = node
                    + DELIMITER + itin.getSource()
                    + DELIMITER + itin.getTarget()
                    + DELIMITER + itin.getWdayPeakInit()
                    + DELIMITER + itin.getRangePeakInit()
                    + DELIMITER + itin.getWdayPeakFin()
                    + DELIMITER + itin.getRangePeakFin();
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
