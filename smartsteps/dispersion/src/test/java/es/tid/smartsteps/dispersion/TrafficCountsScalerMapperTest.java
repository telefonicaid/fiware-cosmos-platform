package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.data.generated.LookupProtocol.Lookup;
import es.tid.smartsteps.dispersion.parsing.LookupParser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
public class TrafficCountsScalerMapperTest extends TrafficCountsBasedTest {

    private MapDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Message>> instance;
    private TrafficCountsParser parser;
    private Text key;
    private TypedProtobufWritable<Message> countsValue;
    private TypedProtobufWritable<Message> lookupValue;

    public TrafficCountsScalerMapperTest() throws IOException {
    }
    
    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Message>>(
                        new TrafficCountsScalerMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        this.parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = this.parser.parse(this.trafficCounts);
        this.countsValue = new TypedProtobufWritable<Message>(counts);
        LookupParser lookupParser =
                new LookupParser(this.conf.get(Config.DELIMITER));
        this.lookupValue = new TypedProtobufWritable<Message>(
                lookupParser.parse("000012006440,fdsafs,0.37"));
    }

    @Test
    public void testReduceWithTrafficCounts() throws IOException {
        List<Pair<Text, TypedProtobufWritable<Message>>> results =
                this.instance
                        .withInput(this.key, this.countsValue)
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Message>> result = results.get(0);
        assertEquals(this.key.toString(), result.getFirst().toString());
        final Message outValue0 = result.getSecond().get();
        assertTrue(outValue0 instanceof TrafficCounts);
    }

    @Test
    public void testReduceWithLookup() throws IOException {
        List<Pair<Text, TypedProtobufWritable<Message>>> results =
                this.instance
                        .withInput(this.key, this.lookupValue)
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Message>> result = results.get(0);
        assertEquals(this.key.toString(), result.getFirst().toString());
        final Message outValue0 = result.getSecond().get();
        assertTrue(outValue0 instanceof Lookup);
    }
}
