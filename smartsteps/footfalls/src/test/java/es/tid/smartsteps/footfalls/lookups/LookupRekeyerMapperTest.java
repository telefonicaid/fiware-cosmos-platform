package es.tid.smartsteps.footfalls.lookups;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.catchments.CatchmentsBasedTest;
import es.tid.smartsteps.footfalls.config.Config;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;
import es.tid.smartsteps.footfalls.lookups.LookupRekeyerMapper.RekeyBy;

/**
 *
 * @author dmicol
 */
public class LookupRekeyerMapperTest extends CatchmentsBasedTest {

    private Text key;
    private TypedProtobufWritable<Lookup> value;
    private MapDriver<
            Text, TypedProtobufWritable<Lookup>,
            Text, TypedProtobufWritable<Lookup>> instance;

    public LookupRekeyerMapperTest() throws IOException {}

    @Before
    public void setUp() {
        this.instance = new MapDriver<Text, TypedProtobufWritable<Lookup>,
                Text, TypedProtobufWritable<Lookup>>(new LookupRekeyerMapper());
        this.key = new Text("000012006440");
        LookupParser lookupParser =
                new LookupParser(this.conf.get(Config.DELIMITER));
        this.value = new TypedProtobufWritable<Lookup>(
                lookupParser.parse("000012006440,fdsafs,0.37"));
    }

    @Test
    public void testRekeyByKey() throws IOException {
        this.instance.getConfiguration().setEnum(RekeyBy.class.getName(),
                                                 RekeyBy.KEY);
        List<Pair<Text, TypedProtobufWritable<Lookup>>> results = this.instance
                .withInput(this.key, this.value)
                .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Lookup>> result = results.get(0);
        assertEquals("000012006440", result.getFirst().toString());
    }

    @Test
    public void testRekeyByValue() throws IOException {
        this.instance.getConfiguration().setEnum(RekeyBy.class.getName(),
                                                 RekeyBy.VALUE);
        List<Pair<Text, TypedProtobufWritable<Lookup>>> results = this.instance
                .withInput(this.key, this.value)
                .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        final Pair<Text, TypedProtobufWritable<Lookup>> result = results.get(0);
        assertEquals("fdsafs", result.getFirst().toString());
    }

    @Test(expected=IllegalArgumentException.class)
    public void testRekeyByInvalid() throws IOException {
        this.instance
                .withInput(this.key, this.value)
                .runTest();
    }

    @Test(expected=IllegalArgumentException.class)
    public void testMissingConfiguration() throws IOException {
        this.instance
                .withInput(this.key, this.value)
                .runTest();
    }
}
