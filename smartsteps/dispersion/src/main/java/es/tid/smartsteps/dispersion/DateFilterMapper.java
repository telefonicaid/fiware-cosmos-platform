package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
class DateFilterMapper extends Mapper<
        Text, TypedProtobufWritable<TrafficCounts>,
        Text, TypedProtobufWritable<TrafficCounts>> {

    private String dateToFilter;
    
    @Override
    protected void setup(Context context) {
        this.dateToFilter = context.getConfiguration().get(Config.DATE_TO_FILTER);
    }
    
    @Override
    protected void map(Text key, TypedProtobufWritable<TrafficCounts> value,
            Context context) throws IOException, InterruptedException {
        final TrafficCounts counts = value.get();
        if (!counts.getDate().equals(this.dateToFilter)) {
            return;
        }
        context.write(key, value);
    }
}
