package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
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
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 *
 * @author dmicol
 */
public class BorrarGetBtsComareaMapperTest {
    private MapDriver<LongWritable, Text, LongWritable,
            TypedProtobufWritable<Bts>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, Text, LongWritable,
                TypedProtobufWritable<Bts>>(new BorrarGetBtsComareaMapper());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, TypedProtobufWritable<Bts>>> res =
                this.driver
                        .withInput(new LongWritable(1L),
                                   new Text("17360  17360 711.86 6673"))
                        .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        LongWritable key = res.get(0).getFirst();
        assertEquals(17360L, key.get());
        TypedProtobufWritable<Bts> wrappedBts = res.get(0).getSecond();
        assertNotNull(wrappedBts.get());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           new Text("17360  17360 711.86 blah"))
                .runTest();
    }
}
