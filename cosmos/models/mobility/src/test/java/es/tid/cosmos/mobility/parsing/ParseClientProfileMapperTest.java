package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ParseClientProfileMapperTest {
    private MapDriver<LongWritable, Text, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, Text, LongWritable,
                ProtobufWritable<MobData>>(new ParseClientProfileMapper());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, ProtobufWritable<MobData>>> res = this.driver
                .withInput(new LongWritable(1L), new Text("2221436242|12"))
                .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        assertEquals(new LongWritable(2221436242L), res.get(0).getFirst());
        ProtobufWritable<MobData> wrappedCell = res.get(0).getSecond();
        wrappedCell.setConverter(MobData.class);
        assertNotNull(wrappedCell.get().getCell());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           new Text("33F43052|blah blah|4234232"))
                .runTest();
    }
}
