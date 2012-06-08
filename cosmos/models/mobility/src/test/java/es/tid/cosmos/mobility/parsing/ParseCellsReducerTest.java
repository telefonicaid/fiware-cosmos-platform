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

import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ParseCellsReducerTest {
    private ReduceDriver<LongWritable, Text, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        //this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
        //        ProtobufWritable<MobData>>(new ParseCellsReducer());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, ProtobufWritable<MobData>>> res = this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F43052|2221436242|12|34|56|78")))
                .run();
        assertEquals(1, res.size());
        assertEquals(new LongWritable(871641170L), res.get(0).getFirst());
        ProtobufWritable<MobData> wrappedCell = res.get(0).getSecond();
        wrappedCell.setConverter(MobData.class);
        assertNotNull(wrappedCell.get().getCell());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("33F43052|blah blah|4234232")))
                .runTest();
    }
}
