package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Test;
import static org.junit.Assert.assertEquals;
import org.junit.Before;

import es.tid.cosmos.mobility.data.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.TimeUtil;

/**
 *
 * @author dmicol
 */
public class VectorSpreadNodedayhourReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<Cdr>,
            ProtobufWritable<NodeBts>, NullWritable> instance;
    @Before
    public void setUp() throws Exception {
        this.instance = new ReduceDriver<LongWritable, ProtobufWritable<Cdr>,
                ProtobufWritable<NodeBts>, NullWritable>(
                        new VectorSpreadNodedayhourReducer());
    }

    @Test
    public void testReduce() throws IOException {
        Date date1 = DateUtil.create(2012, 4, 11, 3);
        Time time1 = TimeUtil.create(1, 2, 3);
        ProtobufWritable<Cdr> cdr1 = CdrUtil.createAndWrap(12L, 34L, date1,
                                                           time1);
        cdr1.setConverter(Cdr.class);
        Date date2 = DateUtil.create(2012, 4, 13, 5);
        Time time2 = TimeUtil.create(4, 5, 6);
        ProtobufWritable<Cdr> cdr2 = CdrUtil.createAndWrap(56L, 78L, date2,
                                                           time2);
        List<Pair<ProtobufWritable<NodeBts>, NullWritable>> results =
                this.instance
                        .withInput(new LongWritable(1L), asList(cdr1, cdr2))
                        .run();
        assertEquals(2, results.size());
        ProtobufWritable<NodeBts> result1 = results.get(0).getFirst();
        result1.setConverter(NodeBts.class);
        assertEquals(0,  result1.get().getWeekday());
        ProtobufWritable<NodeBts> result2 = results.get(1).getFirst();
        result2.setConverter(NodeBts.class);
        assertEquals(1, result2.get().getWeekday());
    }
}
