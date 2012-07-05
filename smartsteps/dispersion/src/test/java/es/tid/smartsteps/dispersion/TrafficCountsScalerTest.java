package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;

import com.google.protobuf.Message;
import static junit.framework.Assert.assertEquals;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.LookupParser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 * These tests show what is the effect of the microgrid dispersion Job: it
 * scales the input value according to the lookup tables proportion, and it
 * joins the input key with the corresponding lookup values.
 *
 * @author logc
 */
public class TrafficCountsScalerTest {

    private MapReduceDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<TrafficCounts>> instance;
    private Text key;
    private TypedProtobufWritable<Message> inputValue;
    private TypedProtobufWritable<Message> cell2micro1;
    private TypedProtobufWritable<Message> cell2micro2;
    private TypedProtobufWritable<Message> micro2polygon;

    @Before
    public void setUp() throws IOException {
        this.instance = new MapReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<TrafficCounts>>(
                new TrafficCountsScalerMapper(), new TrafficCountsScalerReducer());
        final Configuration config = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                this.instance.getConfiguration());
        this.instance.setConfiguration(config);
        this.key = new Text("033749032183");
        TrafficCountsParser parser = new TrafficCountsParser(
                config.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = parser.parse("{\"date\": \"20120527\", "
                + "\"footfall_observed_basic\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1], "
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
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1], "
                + "\"footfall_observed_age_60\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"footfall_observed_age_20\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"cellid\": \"033749032183\", \"footfall_observed_age_40\": "
                + "[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,"
                + "0, 0, 0, 0, 0]}");
        this.inputValue = new TypedProtobufWritable<Message>(counts);
        LookupParser lookupParser = new LookupParser(config.get(Config.DELIMITER));
        this.cell2micro1 = new TypedProtobufWritable<Message>(
                lookupParser.parse("033749032183|123|0.57"));
        this.cell2micro2 = new TypedProtobufWritable<Message>(
                lookupParser.parse("033749032183|124|0.43"));
        this.micro2polygon = new TypedProtobufWritable<Message>(
                lookupParser.parse("033749032183|345|1"));
    }

    @Test
    public void testIntermediateResults() throws Exception {
        this.instance.getConfiguration().set(Config.DELIMITER, "\\|");
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> allResults =
                this.instance
                        .withInput(this.key, this.inputValue)
                        .withInput(this.key, this.cell2micro1)
                        .withInput(this.key, this.cell2micro2)
                        .run();
        assertEquals(2, allResults.size());
        JSONObject result0 = this.getResult(allResults.get(0));
        assertEquals("123", result0.get("cellid"));
        
        JSONObject result1 = this.getResult(allResults.get(1));
        assertEquals("124", result1.get("cellid"));
        
        JSONArray sum = result0.getJSONArray("footfall_observed_basic");
        final JSONArray result1Array = result1.getJSONArray(
                "footfall_observed_basic");
        for (int i = 0; i < sum.size(); i++) {
            sum.set(i, sum.getDouble(i) + result1Array.getDouble(i));
        }
        
        JSONObject input = (JSONObject) JSONSerializer.toJSON(
                this.inputValue.toString());
        assertEquals(input.getJSONArray("footfall_observed_basic"), sum);
    }
    
    private JSONObject getResult(
            Pair<Text, TypedProtobufWritable<TrafficCounts>> result) {
        return (JSONObject) JSONSerializer.toJSON(
                        result.getSecond().toString());
    }

    @Test
    public void testFinalResults() throws Exception {
        this.instance.getConfiguration().set(Config.DELIMITER, "\\|");
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> allResults =
                this.instance
                        .withInput(this.key, this.inputValue)
                        .withInput(this.key, this.micro2polygon)
                        .run();
        assertEquals(1, allResults.size());
        Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                allResults.get(0);
        JSONObject resultJson = this.getResult(result);
        assertEquals("345", resultJson.get("cellid"));
    }
}
