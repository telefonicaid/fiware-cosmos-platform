package es.tid.cosmos.mobility.parsing;

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

import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class BorrarGetBtsComareaMapperTest {
    private MapDriver<LongWritable, Text, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, Text, LongWritable,
                ProtobufWritable<MobData>>(new BorrarGetBtsComareaMapper());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<LongWritable, ProtobufWritable<MobData>>> res =
                this.driver
                        .withInput(new LongWritable(1L),
                                   new Text("17360  17360 711.86 6673"))
                        .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        LongWritable key = res.get(0).getFirst();
        assertEquals(17360L, key.get());
        ProtobufWritable<MobData> wrappedBts = res.get(0).getSecond();
        wrappedBts.setConverter(MobData.class);
        assertNotNull(wrappedBts.get().getBts());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           new Text("17360  17360 711.86 blah"))
                .runTest();
    }
}
