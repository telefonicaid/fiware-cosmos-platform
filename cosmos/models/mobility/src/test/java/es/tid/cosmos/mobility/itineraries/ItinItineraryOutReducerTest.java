package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;

/**
 *
 * @author dmicol
 */
public class ItinItineraryOutReducerTest extends ConfiguredTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Itinerary>,
            NullWritable, Text> instance;

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<Itinerary>, NullWritable, Text>(
                        new ItinItineraryOutReducer());
        this.instance.setConfiguration(this.getConf());
    }

    @Test
    public void testReduce() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Itinerary> value =
                new TypedProtobufWritable<Itinerary>(
                        Itinerary.getDefaultInstance());
        List<Pair<NullWritable, Text>> results = this.instance
                .withInput(key, Arrays.asList(value, value))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        assertEquals(NullWritable.get(), results.get(0).getFirst());
        assertTrue(results.get(0).getSecond().toString().startsWith("57"));
        assertEquals(NullWritable.get(), results.get(1).getFirst());
        assertTrue(results.get(1).getSecond().toString().startsWith("57"));
    }
}
