package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.parsing.ParseCellsMapper;

/**
 *
 * @author dmicol
 */
public class ParseCellsMapperTest {
    private MapDriver<LongWritable, Text, LongWritable, ProtobufWritable<Cdr>>
            driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, Text, LongWritable,
                ProtobufWritable<Cdr>>(new ParseCellsMapper());
    }

    @Test
    public void test() throws IOException {
        List<Pair<LongWritable, ProtobufWritable<Cdr>>> results = this.driver
                .withInput(new LongWritable(1L),
                           new Text("33F43052|2221436242|12|34|56|78"))
                .run();
        assertEquals(1, results.size());
        assertEquals(new LongWritable(871641170L), results.get(0).getFirst());
        ProtobufWritable<Cdr> wrappedCdr = results.get(0).getSecond();
        wrappedCdr.setConverter(Cdr.class);
        assertNotNull(wrappedCdr.get());
    }
}
