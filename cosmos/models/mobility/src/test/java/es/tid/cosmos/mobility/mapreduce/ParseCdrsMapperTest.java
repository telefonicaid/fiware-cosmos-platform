package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.mapreduce.ParseCdrsMapper;

/**
 *
 * @author dmicol
 */
public class ParseCdrsMapperTest {
    private MapDriver<IntWritable, Text, LongWritable, ProtobufWritable<Cdr>>
            driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<IntWritable, Text, LongWritable,
                ProtobufWritable<Cdr>>(new ParseCdrsMapper());
    }

    @Test
    public void test() throws IOException {
        List<Pair<LongWritable, ProtobufWritable<Cdr>>> results = this.driver
                .withInput(new IntWritable(1),
                           new Text("33F430521676F4|2221436242|"
                                    + "33F430521676F4|0442224173253|2|"
                                    + "01/01/2010|02:00:01|2891|RMITERR"))
                .run();
        assertEquals(1, results.size());
        assertEquals(new LongWritable(2221436242L), results.get(0).getFirst());
        ProtobufWritable<Cdr> wrappedCdr = results.get(0).getSecond();
        wrappedCdr.setConverter(Cdr.class);
        assertNotNull(wrappedCdr.get());
    }
}
