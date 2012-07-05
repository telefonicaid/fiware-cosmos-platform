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
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
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
                final Counts footfallCounts = counts.getFootfalls(i);
                Counts.Builder footfallCountsBuilder =
                        Counts.newBuilder(aggregatedCountsBuilder.getFootfalls(i));
                for (int j = 0; j < footfallCountsBuilder.getValuesCount(); j++) {
                    footfallCountsBuilder.setValues(j,
                            footfallCountsBuilder.getValues(j)
                            + footfallCounts.getValues(j));
                }
                aggregatedCountsBuilder.setFootfalls(i, footfallCountsBuilder);
            }
        }
        this.outKey.set(aggregatedCountsBuilder.getCellId());
        this.aggregatedCounts.set(aggregatedCountsBuilder.build());
        context.write(this.outKey, this.aggregatedCounts);
    }
}
