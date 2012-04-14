package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.parsing.JoinBtsNodeToBtsDayRangeReducer;

/**
 *
 * @author dmicol
 */
public class JoinBtsNodeToBtsDayRangeReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<TwoInt>>(
                        new JoinBtsNodeToBtsDayRangeReducer());
    }
    
    @Test
    public void testSomeMethod() throws IOException {
        Cdr cdr1 = CdrUtil.create(1L, 2L, Date.getDefaultInstance(),
                                  Time.getDefaultInstance());
        Cdr cdr2 = CdrUtil.create(1L, 2L,Date.getDefaultInstance(),
                                  Time.getDefaultInstance());
        Cell cell = CellUtil.create(10L, 20L, 30, 40, 50D, 60D);
        List<Pair<LongWritable, ProtobufWritable<TwoInt>>> results =
                this.driver
                        .withInput(new LongWritable(100L),
                                   asList(MobDataUtil.createAndWrap(cdr1),
                                          MobDataUtil.createAndWrap(cell),
                                          MobDataUtil.createAndWrap(cdr2)))
                        .run();
    }
}
