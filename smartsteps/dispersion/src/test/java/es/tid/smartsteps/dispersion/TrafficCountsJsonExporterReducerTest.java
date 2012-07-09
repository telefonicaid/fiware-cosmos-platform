package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
public class TrafficCountsJsonExporterReducerTest extends TrafficCountsBasedTest {

    private ReduceDriver<
            Text, TypedProtobufWritable<TrafficCounts>,
            NullWritable, Text> instance;
    private TrafficCountsParser parser;
    private Text key;
    private TypedProtobufWritable<TrafficCounts> value;
    
    public TrafficCountsJsonExporterReducerTest() throws IOException {
    }
    
    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<TrafficCounts>,
                NullWritable, Text>(new TrafficCountsJsonExporterReducer());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        this.parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = this.parser.parse(this.trafficCounts);
        this.value = new TypedProtobufWritable<TrafficCounts>(counts);
    }

    @Test
    public void testExportNoRound() throws IOException {
        this.instance.getConfiguration().setBoolean(Config.ROUND_RESULTS, false);
        List<Pair<NullWritable, Text>> results = this.instance
                .withInput(this.key, Arrays.asList(this.value, this.value))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        final Pair<NullWritable, Text> result0 = results.get(0);
        assertEquals(NullWritable.get(), result0.getFirst());
        TrafficCounts counts = this.parser.parse(result0.getSecond().toString());
        assertNotNull(counts);
        assertEquals(0.57D, counts.getFootfalls(0).getValues(23), 0.0D);
    }

    @Test
    public void testExportRound() throws IOException {
        this.instance.getConfiguration().setBoolean(Config.ROUND_RESULTS, true);
        List<Pair<NullWritable, Text>> results = this.instance
                .withInput(this.key, Arrays.asList(this.value, this.value))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        final Pair<NullWritable, Text> result0 = results.get(0);
        assertEquals(NullWritable.get(), result0.getFirst());
        TrafficCounts counts = this.parser.parse(result0.getSecond().toString());
        assertNotNull(counts);
        assertEquals(1.0D, counts.getFootfalls(0).getValues(23), 0.0D);
    }
}
