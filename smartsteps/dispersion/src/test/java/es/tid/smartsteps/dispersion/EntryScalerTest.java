package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;

import static junit.framework.Assert.assertEquals;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

/**
 * These tests show what is the effect of the microgrid dispersion Job: it
 * scales the input value according to the lookup tables proportion, and it
 * joins the input key with the corresponding lookup values.
 *
 * @author logc
 */
public class EntryScalerTest {

    private MapReduceDriver<LongWritable, Text, Text, Text, NullWritable, Text>
            instance;
    private LongWritable key;
    private Text inputValue;
    private Text intermediateValue;
    private Text cell2micro1;
    private Text cell2micro2;
    private Text micro2polygon;

    @Before
    public void setUp() throws IOException {
        this.instance = new MapReduceDriver<LongWritable, Text,
                                            Text, Text,  NullWritable, Text>(
                new EntryScalerMapper(), new EntryScalerReducer());
        final Configuration config = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                this.instance.getConfiguration());
        this.instance.setConfiguration(config);
        this.key = new LongWritable(102L);
        this.inputValue = new Text("{\"date\": \"20120527\", "
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
        int inputPenultimate = this.inputValue.getLength() - 1;
        this.intermediateValue =
                new Text(this.inputValue.toString().substring(0,
                        inputPenultimate) + ", \"microgrid_id\": 123}");

        this.cell2micro1 =
                new Text("033749032183|123|0.57");
        this.cell2micro2 =
                new Text("033749032183|124|0.43");
        this.micro2polygon = new Text("123|345|1");
    }

    @Test
    public void testIntermediateResults() throws Exception {
        this.instance.getConfiguration().setEnum(LookupType.class.getName(),
                                                 LookupType.CELL_TO_MICROGRID);
        this.instance.getConfiguration().set(Config.DELIMITER, "\\|");
        List<Pair<NullWritable, Text>> allResults =
                this.instance.withInput(this.key, this.inputValue)
                    .withInput(new LongWritable(1L), this.cell2micro1)
                    .withInput(new LongWritable(1L), this.cell2micro2)
                    .run();
        assertEquals(2, allResults.size());
        JSONObject result0 = this.getResult(allResults.get(0));
        assertEquals("123", result0.get("microgrid_id"));
        assertEquals("", result0.get("polygon_id"));
        
        
        JSONObject result1 = this.getResult(allResults.get(1));
        assertEquals("124", result1.get("microgrid_id"));
        assertEquals("", result1.get("polygon_id"));
        
        JSONArray sum = result0.getJSONArray("footfall_observed_basic");
        final JSONArray result1Array = result1.getJSONArray("footfall_observed_basic");
        for(int i = 0; i < sum.size(); i++) {
            sum.set(i, sum.getDouble(i) + result1Array.getDouble(i));
        }
        
        JSONObject input = (JSONObject) JSONSerializer.toJSON(this.inputValue.toString());
        assertEquals(input.getJSONArray("footfall_observed_basic"), sum);
    }
    
    private JSONObject getResult(Pair<NullWritable, Text> result) {
        return (JSONObject) JSONSerializer.toJSON(
                        result.getSecond().toString());
    }

    @Test
    public void testFinalResults() throws Exception {
        this.instance.getConfiguration().setEnum(LookupType.class.getName(),
                                             LookupType.MICROGRID_TO_POLYGON);
        this.instance.getConfiguration().set(Config.DELIMITER, "\\|");
        List<Pair<NullWritable, Text>> allResults =
                this.instance.withInput(this.key, this.intermediateValue)
                        .withInput(new LongWritable(1L), this.micro2polygon)
                        .run();
        assertEquals(1, allResults.size());
        Pair<NullWritable, Text> result = allResults.get(0);
        JSONObject resultJson =
                (JSONObject) JSONSerializer.toJSON(
                        result.getSecond().toString());
        assertEquals("123", resultJson.get("microgrid_id"));
        assertEquals("345", resultJson.get("polygon_id"));
    }
}
