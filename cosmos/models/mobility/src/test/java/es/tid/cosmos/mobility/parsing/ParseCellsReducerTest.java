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
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class ParseCellsReducerTest {
    private ReduceDriver<LongWritable, Text, LongWritable,
            MobilityWritable<Cell>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
                MobilityWritable<Cell>>(new ParseCellsReducer());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, MobilityWritable<Cell>>> res = this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F43052|2221436242|12|34|56|78")))
                .run();
        assertEquals(1, res.size());
        assertEquals(new LongWritable(871641170L), res.get(0).getFirst());
        MobilityWritable<Cell> wrappedCell = res.get(0).getSecond();
        assertNotNull(wrappedCell.get());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F43052|blah blah|4234232")))
                .runTest();
    }
}
