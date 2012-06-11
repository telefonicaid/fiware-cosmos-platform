package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>, LongWritable,
            MobilityWritable<Cdr>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                LongWritable, MobilityWritable<Cdr>>(
                        new VectorFiltClientsReducer());
    }

    @Test
    public void testNoNumberOfCommunications() throws IOException {
        MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                Cdr.getDefaultInstance());
        MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                Cdr.getDefaultInstance());
        List<Pair<LongWritable, MobilityWritable<Cdr>>> res = this.driver
                .withInput(new LongWritable(17L), asList(value1, value2))
                .run();
        assertNotNull(res);
        assertEquals(0, res.size());
    }
    
    @Test(expected=IllegalStateException.class)
    public void testInvalidNumberOfCommunications() throws IOException {
        MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                Int.newBuilder().setNum(56).build());
        MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                Int.newBuilder().setNum(137).build());
        this.driver
                .withInput(new LongWritable(17L), asList(value1, value2))
                .run();
    }
}
