package es.tid.cosmos.mobility.parsing;

import java.io.IOException;
import static java.util.Arrays.asList;
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
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjParseAdjBtsReducerTest {
    private ReduceDriver<LongWritable, Text, ProtobufWritable<TwoInt>,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        //this.driver = new ReduceDriver<LongWritable, Text,
        //        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>(
        //                new AdjParseAdjBtsReducer());
    }

    @Test
    public void testValidLine() throws IOException {
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>> res =
                this.driver
                        .withInput(new LongWritable(1L),
                                   asList(new Text("123|456")))
                        .run();
        assertEquals(1, res.size());
        ProtobufWritable<TwoInt> wrappedKey = res.get(0).getFirst();
        wrappedKey.setConverter(TwoInt.class);
        assertNotNull(wrappedKey.get());
        ProtobufWritable<MobData> wrappedNull = res.get(0).getSecond();
        wrappedNull.setConverter(MobData.class);
        assertEquals(MobData.Type.NULL, wrappedNull.get().getType());
    }

    @Test
    public void testInvalidLine() throws IOException {
        this.driver
                .withInput(new LongWritable(1L),
                           asList(new Text("2221436242|blah blah|1234")))
                .runTest();
    }
}
