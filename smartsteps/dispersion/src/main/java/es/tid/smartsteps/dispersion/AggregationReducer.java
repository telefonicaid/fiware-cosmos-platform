package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
class AggregationReducer extends Reducer<
        BinaryKey, TypedProtobufWritable<TrafficCounts>,
        Text, TypedProtobufWritable<TrafficCounts>> {

    private Text outKey;
    private TypedProtobufWritable<TrafficCounts> aggregatedCounts;
    
    @Override
    protected void setup(Context context) {
        this.outKey = new Text();
        this.aggregatedCounts = new TypedProtobufWritable<TrafficCounts>();
    }
    
    @Override
    protected void reduce(BinaryKey key,
            Iterable<TypedProtobufWritable<TrafficCounts>> values,
            Context context) throws IOException, InterruptedException {
        TrafficCounts.Builder aggregatedCountsBuilder = null;
        for (TypedProtobufWritable<TrafficCounts> value : values) {
            final TrafficCounts counts = value.get();
            if (aggregatedCountsBuilder == null) {
                aggregatedCountsBuilder = TrafficCounts.newBuilder(counts);
                continue;
            }
            for (int i = 0; i < counts.getFootfallsCount(); i++) {
                aggregatedCountsBuilder.setFootfalls(i,
                        aggregateCounts(aggregatedCountsBuilder.getFootfalls(i),
                                        counts.getFootfalls(i)));
            }
            for (int i = 0; i < counts.getPoisCount(); i++) {
                aggregatedCountsBuilder.setPois(i,
                        aggregateCounts(aggregatedCountsBuilder.getPois(i),
                                        counts.getPois(i)));
            }
        }
        this.outKey.set(aggregatedCountsBuilder.getCellId());
        this.aggregatedCounts.set(aggregatedCountsBuilder.build());
        context.write(this.outKey, this.aggregatedCounts);
    }
    
    private static Counts aggregateCounts(Counts a, Counts b) {
        if (!a.getName().equals(b.getName()) ||
                a.getValuesCount() != b.getValuesCount()) {
            throw new IllegalArgumentException(
                    "Counts to be aggregated are not compatible");
        }
        Counts.Builder aggregatedCounts = Counts.newBuilder(a);
        for (int i = 0; i < aggregatedCounts.getValuesCount(); i++) {
            aggregatedCounts.setValues(i,
                    aggregatedCounts.getValues(i) + b.getValues(i));
        }
        return aggregatedCounts.build();
    }
}
