package es.tid.smartsteps.dispersion;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
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
import es.tid.smartsteps.dispersion.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class LookupParserMapperTest {

    private MapDriver<
            LongWritable, Text,
            Text, TypedProtobufWritable<Lookup>> instance;
    private LongWritable key;
    private Text value;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new MapDriver<
                LongWritable, Text,
                Text, TypedProtobufWritable<Lookup>>(new LookupParserMapper());
        final Configuration config = Config.load(
                Config.class.getResource("/config.properties").openStream(),
                this.instance.getConfiguration());
        this.instance.setConfiguration(config);
        this.key = new LongWritable(102L);
        this.value = new Text("sdfasgsa|fdsfasr|0.57");
    }

    @Test
    public void shouldProduceOutput() throws IOException {
        List<Pair<Text, TypedProtobufWritable<Lookup>>> results = this.instance
                .withInput(this.key, this.value)
                .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Lookup>> result = results.get(0);
        assertEquals("sdfasgsa", result.getFirst());
        assertTrue(result.getSecond().get() instanceof Lookup);
    }
    
    @Test
    public void shouldFailToParse() {
        this.instance
                .withInput(this.key, new Text("blah blah"))
                .runTest();
    }
}
