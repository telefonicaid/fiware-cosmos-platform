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
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 *
 * @author dmicol
 */
public class BorrarGetBtsComareaReducerTest {
    private ReduceDriver<LongWritable, Text, LongWritable,
            MobilityWritable<Bts>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
                MobilityWritable<Bts>>(new BorrarGetBtsComareaReducer());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, MobilityWritable<Bts>>> res =
                this.driver
                        .withInput(new LongWritable(1L),
                                   asList(new Text("17360  17360 711.86 6673")))
                        .run();
        assertEquals(1, res.size());
        LongWritable key = res.get(0).getFirst();
        assertEquals(17360L, key.get());
        MobilityWritable<Bts> wrappedBts = res.get(0).getSecond();
        assertNotNull(wrappedBts.get());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("17360  17360 711.86 blah")))
                .runTest();
    }
}
