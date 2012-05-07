package es.tid.cosmos.mobility.parsing;

import static java.util.Arrays.asList;
import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class ParseCellsReducerTest {
    private ReduceDriver<LongWritable, Text, LongWritable,
            ProtobufWritable<Cell>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
                ProtobufWritable<Cell>>(new ParseCellsReducer());
    }

    @Test
    public void test() throws IOException {
        List<Pair<LongWritable, ProtobufWritable<Cell>>> results = this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F43052|2221436242|12|34|56|78")))
                .run();
        assertEquals(1, results.size());
        assertEquals(new LongWritable(871641170L), results.get(0).getFirst());
        ProtobufWritable<Cell> wrappedCell = results.get(0).getSecond();
        wrappedCell.setConverter(Cell.class);
        assertNotNull(wrappedCell.get());
    }
}
