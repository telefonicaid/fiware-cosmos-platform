package es.tid.smartsteps.dispersion;

import java.util.*;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 * Sum up several serialized TrafficCounts adding its counters.
 *
 * @author sortega
 */
public final class TrafficCountsAggregator {

    private TrafficCountsAggregator() {}

    /**
     * Unwrap and sum vectors.
     *
     * @param values  Values to be aggregated
     * @return        Aggregated value
     */
    public static TrafficCounts aggregateWrapped(
            Iterable<TypedProtobufWritable<TrafficCounts>> values) {
        List<TrafficCounts> unwrappedValues = new LinkedList<TrafficCounts>();
        for (TypedProtobufWritable<TrafficCounts> value : values) {
            unwrappedValues.add(value.get());
        }
        return aggregate(unwrappedValues);
    }

    /**
     * Aggregate vectors of a number of TrafficCounts. The rest of the fields
     * are taken from the first TrafficCount.
     *
     * @param values  Values to be aggregated
     * @return        Aggregated value
     */
    public static TrafficCounts aggregate(Iterable<TrafficCounts> values) {
        Iterator<TrafficCounts> iterator = values.iterator();
        TrafficCounts firstValue = iterator.next();
        if (!iterator.hasNext()) {
            return firstValue;
        }

        Map<String, Counts> aggregatedVectors = new HashMap<String, Counts>();
        for (Counts vector : firstValue.getVectorsList()) {
            aggregatedVectors.put(vector.getName(), vector);
        }

        while (iterator.hasNext()) {
            final TrafficCounts value = iterator.next();
            for (Counts vector : value.getVectorsList()) {
                final String name = vector.getName();
                Counts aggregatedVector;
                if (aggregatedVectors.containsKey(name)) {
                    aggregatedVector = aggregateCounts(vector,
                            aggregatedVectors.get(name));
                } else {
                    aggregatedVector = vector;
                }
                aggregatedVectors.put(name, aggregatedVector);
            }
        }

        return TrafficCounts.newBuilder(firstValue)
                .clearVectors()
                .addAllVectors(aggregatedVectors.values())
                .build();
    }

    private static Counts aggregateCounts(Counts a, Counts b) {
        Counts.Builder aggregatedCounts = Counts.newBuilder()
                .setName(a.getName());
        for (int i = 0; i < a.getValuesCount(); i++) {
            aggregatedCounts.addValues(a.getValues(i) + b.getValues(i));
        }
        return aggregatedCounts.build();
    }
}
