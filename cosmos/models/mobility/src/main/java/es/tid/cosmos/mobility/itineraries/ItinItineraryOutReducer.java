package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;

/**
 * Input: <Long, Itinerary>
 * Output: <Null, Text>
 *
 * @author dmicol
 */
class ItinItineraryOutReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Itinerary>, NullWritable, Text> {
    private String separator;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Itinerary>> values, Context context)
            throws IOException, InterruptedException {
        final long node = key.get();
        for (TypedProtobufWritable<Itinerary> value : values) {
            final Itinerary itin = value.get();
            String output = node
                    + this.separator + itin.getSource()
                    + this.separator + itin.getTarget()
                    + this.separator + itin.getWdayPeakInit()
                    + this.separator + itin.getRangePeakInit()
                    + this.separator + itin.getWdayPeakFin()
                    + this.separator + itin.getRangePeakFin();
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
