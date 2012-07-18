package es.tid.smartsteps.footfalls.lookups;

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
import es.tid.smartsteps.footfalls.ConfigurationTestBase;
import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class TransitiveLookupReducerTest extends ConfigurationTestBase {

    private ReduceDriver<Text, TypedProtobufWritable<Lookup>,
            Text, TypedProtobufWritable<Lookup>> instance;
    private Text key;
    private TypedProtobufWritable<Lookup> cellToMicrogridLookup;
    private TypedProtobufWritable<Lookup> microgridToPolygonLookup1;
    private TypedProtobufWritable<Lookup> microgridToPolygonLookup2;

    public TransitiveLookupReducerTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<Text, TypedProtobufWritable<Lookup>,
                Text, TypedProtobufWritable<Lookup>>(
                        new TransitiveLookupReducer());
        LookupParser lookupParser =
                new LookupParser(this.conf.get(Config.DELIMITER));
        this.key = new Text("abc");
        this.cellToMicrogridLookup = new TypedProtobufWritable<Lookup>(
                lookupParser.parse("000012006440,abc,0.37"));
        this.microgridToPolygonLookup1 = new TypedProtobufWritable<Lookup>(
                lookupParser.parse("abc,123,0.53"));
        this.microgridToPolygonLookup2 = new TypedProtobufWritable<Lookup>(
                lookupParser.parse("abc,456,0.49"));
    }

    @Test
    public void testReduce() throws IOException {
        List<Pair<Text, TypedProtobufWritable<Lookup>>> results = this.instance
                .withInput(this.key, Arrays.asList(this.cellToMicrogridLookup,
                        this.microgridToPolygonLookup1,
                        this.microgridToPolygonLookup2))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        final Lookup result1 = results.get(0).getSecond().get();
        assertEquals("000012006440", result1.getKey());
        assertEquals("123", result1.getValue());
        assertEquals(0.37D * 0.53D, result1.getProportion(), 0.0D);
        final Lookup result2 = results.get(1).getSecond().get();
        assertEquals("000012006440", result2.getKey());
        assertEquals("456", result2.getValue());
        assertEquals(0.37D * 0.49D, result2.getProportion(), 0.0D);
    }

    @Test
    public void testMissingLookups() throws IOException {
        this.instance
                .withInput(this.key, Arrays.asList(this.cellToMicrogridLookup))
                .runTest();

        this.instance
                .withInput(this.key, Arrays.asList(
                        this.microgridToPolygonLookup1,
                        this.microgridToPolygonLookup2))
                .runTest();
    }

    @Test(expected=IllegalArgumentException.class)
    public void testInvalidKey() throws IOException {
        this.instance
                .withInput(new Text("blah"), Arrays.asList(
                        this.cellToMicrogridLookup,
                        this.microgridToPolygonLookup1,
                        this.microgridToPolygonLookup2))
                .run();
    }
}
