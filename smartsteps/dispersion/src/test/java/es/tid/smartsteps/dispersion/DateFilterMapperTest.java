package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
public class DateFilterMapperTest extends BaseTest {

    private MapDriver<
            Text, TypedProtobufWritable<TrafficCounts>,
            Text, TypedProtobufWritable<TrafficCounts>> instance;
    private Text key;
    private TypedProtobufWritable<TrafficCounts> value;
    
    public DateFilterMapperTest() throws IOException {
    }
    
    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                Text, TypedProtobufWritable<TrafficCounts>,
                Text, TypedProtobufWritable<TrafficCounts>>(
                        new DateFilterMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        TrafficCountsParser parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = parser.parse(this.trafficCounts);
        this.value = new TypedProtobufWritable<TrafficCounts>(counts);
    }

    @Test
    public void testFilterMatchingDate() {
        this.instance.getConfiguration().set(Config.DATE_TO_FILTER, "20120527");
        this.instance
                .withInput(this.key, this.value)
                .withOutput(this.key, this.value)
                .runTest();
    }
    
    @Test
    public void testFilterNonMatchingDate() {
        this.instance.getConfiguration().set(Config.DATE_TO_FILTER, "20120107");
        this.instance
                .withInput(this.key, this.value)
                .runTest();
    }
}
