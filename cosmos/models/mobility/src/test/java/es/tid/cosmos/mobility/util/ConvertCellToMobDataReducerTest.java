package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ConvertCellToMobDataReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<Cell>, LongWritable,
            ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<Cell>,
                LongWritable, ProtobufWritable<MobData>>(
                        new ConvertCellToMobDataReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        LongWritable key = new LongWritable(3);
        Cell cell = CellUtil.create(1L, 2L, 3, 4, 5D, 6D);
        ProtobufWritable<Cell> value = CellUtil.wrap(cell);
        List<Pair<LongWritable, ProtobufWritable<MobData>>> out =
                this.driver
                        .withInput(key, asList(value))
                        .run();
        assertNotNull(out);
        assertEquals(1, out.size());
        assertEquals(key, out.get(0).getFirst());
        ProtobufWritable<MobData> outValue1 = out.get(0).getSecond();
        outValue1.setConverter(MobData.class);
        assertEquals(MobData.Type.CELL, outValue1.get().getType());
        assertEquals(cell, outValue1.get().getCell());
    }
}
