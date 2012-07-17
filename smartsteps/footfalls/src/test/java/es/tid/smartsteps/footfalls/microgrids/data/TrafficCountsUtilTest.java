package es.tid.smartsteps.footfalls.microgrids.data;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Counts.Builder;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author sortega
 */
public class TrafficCountsUtilTest {

    private TrafficCounts trafficCounts;
    private Counts vectorA;

    @Before
    public void setUp() {
        Builder vectorABuilder = Counts.newBuilder()
                .setName("VectorA");
        for (int i = 0; i <= 24; i++) {
            vectorABuilder.addValues(i);
        }
        this.vectorA = vectorABuilder.build();
        this.trafficCounts = TrafficCounts.newBuilder()
                .setId("1")
                .setDate("20121010")
                .setLatitude(0d)
                .setLongitude(0d)
                .addVectors(this.vectorA)
                .build();
    }

    @Test
    public void shouldReturnMatchingVector() {
        assertEquals(this.vectorA,
                     TrafficCountsUtil.getVector(trafficCounts, "VectorA"));
    }

    @Test
    public void shouldReturnNullWhenNotFound() {
        assertNull(TrafficCountsUtil.getVector(trafficCounts, "VectorB"));
    }
}
