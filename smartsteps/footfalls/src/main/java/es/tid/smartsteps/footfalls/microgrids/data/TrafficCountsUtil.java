package es.tid.smartsteps.footfalls.microgrids.data;

import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author sortega
 */
public final class TrafficCountsUtil {

    private TrafficCountsUtil() {}

    /**
     * Looks up a vector by name.
     *
     * @param trafficCounts
     * @param vectorName     Name of the vector
     * @return               The vector or null when not found
     */
    public static Counts getVector(TrafficCounts trafficCounts,
                                   String vectorName) {
        for (Counts vector : trafficCounts.getVectorsList()) {
            if (vectorName.equals(vector.getName())) {
                return vector;
            }
        }
        return null;
    }
}
