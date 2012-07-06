package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.SOACentroid;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.SOACentroidParser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
public class SOACentroidJoinerMapperTest {

    private MapDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Message>> instance;
    private TrafficCountsParser parser;
    private Text key;
    private TypedProtobufWritable<Message> countsValue;
    private TypedProtobufWritable<Message> centroidValue;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Message>>(
                        new SOACentroidJoinerMapper());
        final Configuration config = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                this.instance.getConfiguration());
        this.instance.setConfiguration(config);
        this.key = new Text("soa123");
        this.parser = new TrafficCountsParser(
                config.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = this.parser.parse("{\"date\": \"20120527\", "
                + "\"footfall_observed_basic\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1], "
                + "\"footfall_observed_female\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,"
                + " 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], \"easting\": "
                + "\"125053\", \"poi_5\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], \"lat\": 53.801087"
                + ", \"long\": 1.566688, \"footfall_observed_male\""
                + ": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "1, 0, 0, 0, 0, 1], \"footfall_observed_age_70\": [0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0], \"footfall_observed_age_30\": [0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"footfall_observed_age_50\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],  \"pois\": "
                + "{\"HOME\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 1, 0, 0, 0, 0, 1], \"NONE\": [0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"WORK\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0], \"OTHER\": [0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"BILL\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0]}, "
                + "\"footfall_observed_0\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1], "
                + "\"footfall_observed_age_60\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"footfall_observed_age_20\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"cellid\": \"soa123\", \"footfall_observed_age_40\": [0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0]}");
        this.countsValue = new TypedProtobufWritable<Message>(counts);
        SOACentroidParser soaCentroidParser = new SOACentroidParser(
                config.get(Config.DELIMITER));
        this.centroidValue = new TypedProtobufWritable<Message>(
                soaCentroidParser.parse("soa123,0.3,0.5,0.4"));
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
                        .withInput(this.key, this.centroidValue)
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Message>> result = results.get(0);
        assertEquals(this.key.toString(), result.getFirst().toString());
        final Message outValue0 = result.getSecond().get();
        assertTrue(outValue0 instanceof SOACentroid);
    }
}
