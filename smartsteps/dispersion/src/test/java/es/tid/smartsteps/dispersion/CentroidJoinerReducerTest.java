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
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.CentroidParser;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
public class CentroidJoinerReducerTest extends TrafficCountsBasedTest {

    private ReduceDriver<
            Text, TypedProtobufWritable<Message>,
            Text, TypedProtobufWritable<TrafficCounts>> instance;
    private TrafficCountsParser parser;
    private Text key;
    private TypedProtobufWritable<Message> countsValue;
    private TypedProtobufWritable<Message> centroidValue;

    public CentroidJoinerReducerTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<
                Text, TypedProtobufWritable<Message>,
                Text, TypedProtobufWritable<TrafficCounts>>(
                        new CentroidJoinerReducer());
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
    public void testReduce() throws IOException {
        List<Pair<Text, TypedProtobufWritable<TrafficCounts>>> results =
                this.instance
                        .withInput(this.key,
                                   Arrays.asList(this.countsValue,
                                                 this.centroidValue))
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<TrafficCounts>> result =
                results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
        final TrafficCounts outValue = result.getSecond().get();
        List<Double> counts = outValue.getVectorsList().get(0).getValuesList();
        assertEquals(0, counts.get(15).doubleValue(), 0.0D);
        assertEquals(0.4D, outValue.getLatitude(), 0.0D);
        assertEquals(0.5D, outValue.getLongitude(), 0.0D);
    }
}
