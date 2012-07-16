package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.config.Config;
import es.tid.smartsteps.dispersion.data.TrafficCountsUtil;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.LookupParser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
public class TrafficCountsScalerReducerTest extends TrafficCountsBasedTest {

    private ReduceDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<TrafficCounts>> instance;
    private TrafficCountsParser parser;
    private Text key;
    private TypedProtobufWritable<Message> countsValue;
    private TypedProtobufWritable<Message> lookupValue;

    public TrafficCountsScalerReducerTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<TrafficCounts>>(
                        new TrafficCountsScalerReducer());
        this.instance.setConfiguration(this.conf);
        this.key = new Text("000012006440");
        this.parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = this.parser.parse(this.trafficCounts);
        this.countsValue = new TypedProtobufWritable<Message>(counts);
        LookupParser lookupParser =
                new LookupParser(this.conf.get(Config.DELIMITER));
        this.lookupValue = new TypedProtobufWritable<Message>(
                lookupParser.parse("000012006440,polygon123,0.37"));
    }

    @Test
    public void testReduce() throws IOException {
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> results =
                this.instance
                        .withInput(this.key,
                                   Arrays.asList(this.countsValue,
                                                 this.lookupValue))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                results.get(0);
        assertEquals("polygon123", result.getFirst().toString());
        final TrafficCounts outValue = result.getSecond().get();
        List<Double> counts = TrafficCountsUtil
                .getVector(outValue, "footfall_observed_basic")
                .getValuesList();
        assertEquals(0, counts.get(15).doubleValue(), 0.0D);
        assertEquals(0.74D, counts.get(19).doubleValue(), 0.0D);
        assertEquals(0.37D, counts.get(24).doubleValue(), 0.0D);
    }
}
