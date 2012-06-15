package es.tid.smartsteps.dispersion;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * EntryScalerTest
 *
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
    private Text outputValue;
    private Text cell2micro;

    @Before
    public void setUp() {
        this.instance = new MapReduceDriver<LongWritable, Text,
                                            Text, Text,  NullWritable, Text>(
                            new EntryScalerMapper(), new EntryScalerReducer());
        this.key = new LongWritable(102L);
        this.inputValue = new Text("{\"date\": \"20120527\", "
                + "\"footfall_observed_basic\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1], "
                + "\"footfall_observed_female\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,"
                + " 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], \"easting\": "
                + "\"125053\", \"poi_5\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], \"northing\": "
                + "\"560652\", \"footfall_observed_male\": [0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1], "
                + "\"footfall_observed_age_70\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,"
                + " 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"footfall_observed_age_30\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,"
                + " 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"footfall_observed_age_50\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],  \"pois\": "
                + "{\"HOME\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 1, 0, 0, 0, 0, 1], \"NONE\": [0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"WORK\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0], \"OTHER\": [0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"BILL\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0]}, \"lat\": 54.864832684094964, "
                + "\"footfall_observed_age_0\": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1], "
                + "\"footfall_observed_age_60\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"footfall_observed_age_20\": [0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0], "
                + "\"cellid\": \"4c92f73d4ff50489d8b3e8707d95ddf073fb81aac6d0d3"
                + "0f1f2ff3cdc0849b0c\", \"footfall_observed_age_40\": [0, 0, "
                + "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "
                + "0, 0, 0]}");
        int inputPenultimate = this.inputValue.getLength() - 1;
        this.intermediateValue =
                new Text(this.inputValue.toString().substring(0,
                        inputPenultimate) + ", \"microgidid\": 123}");
        this.outputValue =
                new Text(this.inputValue.toString().substring(0,
                         inputPenultimate) + ", \"microgridid\": 123, " +
                         "\"polygonid\": 345");

        this.cell2micro =
                new Text("4c92f73d4ff50489d8b3e8707d95ddf073fb81aac6d0d3" +
                        "|123|0.57");
    }

    @Test
    public void testIntermediateResults() throws Exception {
        this.instance.getConfiguration().setEnum(LookupType.class.getName(),
                                                 LookupType.CELL_TO_MICROGRID);
        this.instance.getConfiguration().set(Config.DELIMITER, "\\|");
        this.instance.withInput(this.key, this.inputValue)
                     .withInput(new LongWritable(1L), this.cell2micro)
                     .withOutput(NullWritable.get(), this.intermediateValue)
                     .runTest();
    }
}
