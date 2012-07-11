package es.tid.smartsteps.dispersion.parsing;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.TrafficCountsBasedTest;
import es.tid.smartsteps.dispersion.config.Config;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
public class TrafficCountsParserMapperTest extends TrafficCountsBasedTest {

    private MapDriver<
            LongWritable, Text,
            Text, TypedProtobufWritable<TrafficCounts>> instance;
    private LongWritable key;
    
    public TrafficCountsParserMapperTest() throws IOException {
    }
    
    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                LongWritable, Text,
                Text, TypedProtobufWritable<TrafficCounts>>(
                        new TrafficCountsParserMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new LongWritable(102L);
    }

    @Test
    public void shouldProduceOutputNoDate() throws IOException {
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> results =
                this.instance
                        .withInput(this.key, new Text(this.trafficCounts))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
        assertTrue(result.getSecond().get() instanceof TrafficCounts);
    }

    @Test
    public void shouldProduceOutputEmptyDate() throws IOException {
        this.instance.getConfiguration().set(Config.DATE_TO_FILTER, "");
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> results =
                this.instance
                        .withInput(this.key, new Text(this.trafficCounts))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
        assertTrue(result.getSecond().get() instanceof TrafficCounts);
    }
    
    @Test
    public void shouldProduceOutputMatchingDate() throws IOException {
        this.instance.getConfiguration().set(Config.DATE_TO_FILTER, "20120527");
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> results =
                this.instance
                        .withInput(this.key, new Text(this.trafficCounts))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
        assertTrue(result.getSecond().get() instanceof TrafficCounts);
    }

    @Test
    public void shouldNotProduceOutputNonMatchingDate() throws IOException {
        this.instance.getConfiguration().set(Config.DATE_TO_FILTER, "20110418");
        this.instance
                .withInput(this.key, new Text(this.trafficCounts))
                .runTest();
    }
    
    @Test
    public void shouldFailToParse() {
        this.instance
                .withInput(this.key, new Text("blah blah"))
                .runTest();
    }
}
