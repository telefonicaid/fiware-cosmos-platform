package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

/**
 *
 * @author dmicol
 */
public class ParseClientProfileMapperTest {
    private MapDriver<LongWritable, Text, LongWritable,
            TypedProtobufWritable<ClientProfile>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new MapDriver<LongWritable, Text, LongWritable,
                TypedProtobufWritable<ClientProfile>>(new ParseClientProfileMapper());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.driver.setConfiguration(Config.load(configInput,
                                                 this.driver.getConfiguration()));
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, TypedProtobufWritable<ClientProfile>>> res = this.driver
                .withInput(new LongWritable(1L), new Text("2221436242|12"))
                .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        assertEquals(new LongWritable(2221436242L), res.get(0).getFirst());
        TypedProtobufWritable<ClientProfile> wrappedCell = res.get(0).getSecond();
        assertNotNull(wrappedCell.get());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           new Text("33F43052|blah blah|4234232"))
                .runTest();
    }
}
