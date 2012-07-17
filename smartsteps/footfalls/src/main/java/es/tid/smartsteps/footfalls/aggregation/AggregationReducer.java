package es.tid.smartsteps.footfalls.aggregation;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsAggregator;

/**
 * Input: <[id, day], TrafficCount>
 * Output: <[id, day], TrafficCount>
 *
 * @author dmicol, sortega
 */
public class AggregationReducer extends Reducer<
        BinaryKey, TypedProtobufWritable<TrafficCounts>,
        Text, TypedProtobufWritable<TrafficCounts>> {

    private Text outKey;
    private TypedProtobufWritable<TrafficCounts> outValue;

    @Override
    protected void setup(Context context) {
        this.outKey = new Text();
        this.outValue = new TypedProtobufWritable<TrafficCounts>();
    }

    @Override
    protected void reduce(BinaryKey key,
            Iterable<TypedProtobufWritable<TrafficCounts>> values,
            Context context) throws IOException, InterruptedException {
        final TrafficCounts aggregatedCounts =
                TrafficCountsAggregator.aggregateWrapped(values);
        this.outValue.set(aggregatedCounts);
        this.outKey.set(aggregatedCounts.getId());
        context.write(this.outKey, this.outValue);
    }
}
