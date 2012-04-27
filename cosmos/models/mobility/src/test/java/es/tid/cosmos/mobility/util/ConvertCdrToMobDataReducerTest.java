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

import es.tid.cosmos.mobility.data.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ConvertCdrToMobDataReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<Cdr>, LongWritable,
            ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<Cdr>,
                LongWritable, ProtobufWritable<MobData>>(
                        new ConvertCdrToMobDataReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        LongWritable key = new LongWritable(3);
        Cdr cdr = CdrUtil.create(1L, 2L, Date.getDefaultInstance(),
                Time.getDefaultInstance());
        ProtobufWritable<Cdr> value = CdrUtil.wrap(cdr);
        List<Pair<LongWritable, ProtobufWritable<MobData>>> out =
                this.driver
                        .withInput(key, asList(value))
                        .run();
        assertNotNull(out);
        assertEquals(1, out.size());
        assertEquals(key, out.get(0).getFirst());
        ProtobufWritable<MobData> outValue1 = out.get(0).getSecond();
        outValue1.setConverter(MobData.class);
        assertEquals(MobData.Type.CDR, outValue1.get().getType());
        assertEquals(cdr, outValue1.get().getCdr());
    }
}
