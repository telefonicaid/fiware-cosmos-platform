package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class ParseCdrsReducerTest {
    private ReduceDriver<LongWritable, Text, LongWritable,
            MobilityWritable<Cdr>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
                MobilityWritable<Cdr>>(new ParseCdrsReducer());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, MobilityWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F430521676F4|2221436242|"
                                  + "33F430521676F4|0442224173253|2|"
                                  + "01/01/2010|02:00:01|2891|RMITERR")))
                .run();
        assertEquals(1, res.size());
        assertEquals(new LongWritable(2221436242L), res.get(0).getFirst());
        MobilityWritable<Cdr> wrappedCdr = res.get(0).getSecond();
        assertNotNull(wrappedCdr.get());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F430521676F4|blah blah|43242")))
                .runTest();
    }
}
