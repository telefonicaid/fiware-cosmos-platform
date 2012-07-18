package es.tid.smartsteps.footfalls.aggregation;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsParser;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsTestBase;

/**
 *
 * @author dmicol
 */
public class CellIdAndDateMapperTest extends TrafficCountsTestBase {

    private MapDriver<
            Text, TypedProtobufWritable<TrafficCounts>,
            BinaryKey, TypedProtobufWritable<TrafficCounts>> instance;
    private Text key;
    private TypedProtobufWritable<TrafficCounts> value;
    private BinaryKey outKey;

    public CellIdAndDateMapperTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                Text, TypedProtobufWritable<TrafficCounts>,
                BinaryKey, TypedProtobufWritable<TrafficCounts>>(
                        new CellIdAndDateMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        this.outKey = new BinaryKey("000012006440", "20120527");
        TrafficCountsParser parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = parser.parse(this.trafficCounts);
        this.value = new TypedProtobufWritable<TrafficCounts>(counts);
    }

    @Test
    public void testMap() {
        this.instance
                .withInput(this.key, this.value)
                .withOutput(this.outKey, this.value)
                .runTest();
    }
}
