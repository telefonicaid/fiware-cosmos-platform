package es.tid.smartsteps.footfalls.catchments;

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
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;

/**
 *
 * @author dmicol
 */
public class CatchmentsParserMapperTest extends CatchmentsBasedTest {

    private MapDriver<
            LongWritable, Text,
            Text, TypedProtobufWritable<Catchments>> instance;
    private LongWritable key;
    private Text value;

    public CatchmentsParserMapperTest() throws IOException {}

    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                LongWritable, Text,
                Text, TypedProtobufWritable<Catchments>>(
                        new CatchmentsParserMapper());
        this.instance.setConfiguration(this.conf);
        this.key = new LongWritable(102L);
        this.value = new Text(this.catchments);
    }

    @Test
    public void shouldProduceOutput() throws IOException {
        List<Pair<Text, TypedProtobufWritable<Catchments>>> results =
                this.instance
                        .withInput(this.key, this.value)
                        .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Catchments>> result =
                results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
        assertTrue(result.getSecond().get() instanceof Catchments);
    }

    @Test
    public void shouldFailToParse() {
        this.instance
                .withInput(this.key, new Text("blah blah"))
                .runTest();
    }
}
