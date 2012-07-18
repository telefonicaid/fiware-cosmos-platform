package es.tid.smartsteps.footfalls.aggregation;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.data.TrafficCountsUtil;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsParser;
import es.tid.smartsteps.footfalls.trafficcounts.TrafficCountsTestBase;

/**
 *
 * @author dmicol
 */
public class AggregationReducerTest extends TrafficCountsTestBase {

    private ReduceDriver<
            BinaryKey, TypedProtobufWritable<TrafficCounts>,
            Text, TypedProtobufWritable<TrafficCounts>> instance;
    private TrafficCountsParser parser;
    private BinaryKey key;
    private TypedProtobufWritable<TrafficCounts> value;

    public AggregationReducerTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<
                BinaryKey, TypedProtobufWritable<TrafficCounts>,
                Text, TypedProtobufWritable<TrafficCounts>>(
                        new AggregationReducer());
        this.instance.setConfiguration(this.conf);
        this.key = new BinaryKey("000012006440", "20120527");
        this.parser = new TrafficCountsParser(
                this.conf.getStrings(Config.COUNT_FIELDS));
        final TrafficCounts counts = this.parser.parse(this.trafficCounts);
        this.value = new TypedProtobufWritable<TrafficCounts>(counts);
    }

    @Test
    public void testReduce() throws IOException {
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> results =
                this.instance
                        .withInput(this.key,
                                   Arrays.asList(this.value, this.value,
                                                 this.value))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                results.get(0);
        assertEquals(new Text("000012006440"), result.getFirst());
        final TrafficCounts outValue = result.getSecond().get();
        List<Double> counts = TrafficCountsUtil
                .getVector(outValue, "footfall_observed_basic")
                .getValuesList();
        assertEquals(0, counts.get(15).intValue());
        assertEquals(6, counts.get(19).intValue());
        assertEquals(3, counts.get(24).intValue());
    }
}
