package es.tid.smartsteps.dispersion.parsing;

import net.sf.json.JSONException;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;

/**
 *
 * @author dmicol
 */
public class TrafficCountsEntryParserTest {
    private TrafficCountsEntryParser parser;
    
    @Before
    public void setUp() {
        this.parser = new TrafficCountsEntryParser();
    }
    
    @Test
    public void testParse() {
        final String value = "{\"date\": \"20120527\", "
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
                + "0, 0, 0]}";
        TrafficCountsEntry entry = this.parser.parse(value);
        assertEquals("20120527", entry.date);
    }

    @Test(expected=JSONException.class)
    public void testInvalidEntry() {
        final String value = "blahblah";
        this.parser.parse(value);
    }
}
