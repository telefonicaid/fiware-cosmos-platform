package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Itinerary;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ItinItineraryOutReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, NullWritable, Text> {
    private static final String DELIMITER = "|";
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        final long node = key.get();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final Itinerary itin = mobData.getItinerary();
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
