package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.SOACentroid;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
class SOACentroidJoinerReducer extends Reducer<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<TrafficCounts>> {
    
    private TypedProtobufWritable<TrafficCounts> joinedCounts;
    
    @Override
    protected void setup(Context context) {
        this.joinedCounts = new TypedProtobufWritable<TrafficCounts>();
    }
 
    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, TrafficCounts.class, SOACentroid.class);
        List<TrafficCounts> counts = dividedLists.get(TrafficCounts.class);
        List<SOACentroid> centroids = dividedLists.get(SOACentroid.class);
        if (centroids.size() != 1) {
            throw new IllegalArgumentException("Invalid number of SOA centroids");
        }
        final SOACentroid soaCentroid = centroids.get(0);
        for (TrafficCounts count : counts) {
            TrafficCounts.Builder joinedCountsBuilder =
                    TrafficCounts.newBuilder(count);
            joinedCountsBuilder.setLatitude(soaCentroid.getLatitude());
            joinedCountsBuilder.setLongitude(soaCentroid.getLongitude());
            this.joinedCounts.set(joinedCountsBuilder.build());
            context.write(key, this.joinedCounts);
        }
    }
}
