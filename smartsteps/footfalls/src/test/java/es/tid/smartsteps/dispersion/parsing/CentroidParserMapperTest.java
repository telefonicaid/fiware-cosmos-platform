package es.tid.smartsteps.dispersion.parsing;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.TrafficCountsBasedTest;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.SOACentroid;

/**
 *
 * @author dmicol
 */
public class CentroidParserMapperTest extends TrafficCountsBasedTest {

    private MapDriver<
            LongWritable, Text,
            Text, TypedProtobufWritable<SOACentroid>> instance;
    private LongWritable key;
    private Text value;

    public CentroidParserMapperTest() throws IOException {
    }

    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                LongWritable, Text,
                Text, TypedProtobufWritable<SOACentroid>>(
                        new CentroidParserMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new LongWritable(102L);
        this.value = new Text("000012006440,0.3,0.5,0.4");
    }

    @Test
    public void shouldProduceOutput() throws IOException {
        List<Pair<Text, TypedProtobufWritable<SOACentroid>>> results =
                this.instance
                        .withInput(this.key, this.value)
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<SOACentroid>> result =
                results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
        assertTrue(result.getSecond().get() instanceof SOACentroid);
    }

    @Test
    public void shouldFailToParse() {
        this.instance
                .withInput(this.key, new Text("blah blah"))
                .runTest();
    }
}
