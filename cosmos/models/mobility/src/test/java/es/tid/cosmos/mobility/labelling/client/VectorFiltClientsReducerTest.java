package es.tid.cosmos.mobility.labelling.client;

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

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new VectorFiltClientsReducer());
    }

    @Test
    public void testNoNumberOfCommunications() throws IOException {
        ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                Cdr.getDefaultInstance());
        ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                Cdr.getDefaultInstance());
        List<Pair<LongWritable, ProtobufWritable<MobData>>> res = this.driver
                .withInput(new LongWritable(17L), asList(value1, value2))
                .run();
        assertNotNull(res);
        assertEquals(0, res.size());
    }
    
    @Test(expected=IllegalStateException.class)
    public void testInvalidNumberOfCommunications() throws IOException {
        ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(56);
        ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(137);
        this.driver
                .withInput(new LongWritable(17L), asList(value1, value2))
                .run();
    }
}
