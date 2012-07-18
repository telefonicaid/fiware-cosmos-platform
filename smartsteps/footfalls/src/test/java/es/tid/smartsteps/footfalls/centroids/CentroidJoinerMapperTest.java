package es.tid.smartsteps.footfalls.centroids;

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
import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.SOACentroid;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsParser;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsTestBase;

/**
 *
 * @author dmicol
 */
public class CentroidJoinerMapperTest extends TrafficCountsTestBase {

    private MapDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<Message>> instance;
    private TrafficCountsParser parser;
    private Text key;
    private TypedProtobufWritable<Message> countsValue;
    private TypedProtobufWritable<Message> centroidValue;

    public CentroidJoinerMapperTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<Message>>(new CentroidJoinerMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        this.parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = this.parser.parse(this.trafficCounts);
        this.countsValue = new TypedProtobufWritable<Message>(counts);
        CentroidParser soaCentroidParser = new CentroidParser(
                this.conf.get(Config.DELIMITER));
        this.centroidValue = new TypedProtobufWritable<Message>(
                soaCentroidParser.parse("000012006440,0.3,0.5,0.4"));
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
