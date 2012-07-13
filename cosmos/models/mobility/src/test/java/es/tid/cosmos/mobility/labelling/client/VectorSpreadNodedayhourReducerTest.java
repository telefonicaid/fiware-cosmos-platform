package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TimeUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;

/**
 *
 * @author dmicol
 */
public class VectorSpreadNodedayhourReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Cdr>,
            ProtobufWritable<NodeBts>, TypedProtobufWritable<Null>> instance;
    @Before
    public void setUp() throws Exception {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<Cdr>, ProtobufWritable<NodeBts>,
                TypedProtobufWritable<Null>>(
                        new VectorSpreadNodedayhourReducer());
    }

    @Test
    public void testReduce() throws IOException {
        Date date1 = DateUtil.create(2012, 4, 11, 3);
        Time time1 = TimeUtil.create(1, 2, 3);
        TypedProtobufWritable<Cdr> cdr1 = new TypedProtobufWritable<Cdr>(
                CdrUtil.create(12L, 34L, date1, time1));
        Date date2 = DateUtil.create(2012, 4, 13, 5);
        Time time2 = TimeUtil.create(4, 5, 6);
        TypedProtobufWritable<Cdr> cdr2 = new TypedProtobufWritable<Cdr>(
                CdrUtil.create(56L, 78L, date2, time2));
        List<Pair<ProtobufWritable<NodeBts>, TypedProtobufWritable<Null>>> res =
                this.instance
                        .withInput(new LongWritable(1L), asList(cdr1, cdr2))
                        .run();
        assertEquals(2, res.size());
        ProtobufWritable<NodeBts> result1 = res.get(0).getFirst();
        result1.setConverter(NodeBts.class);
        assertEquals(0,  result1.get().getWeekday());
        ProtobufWritable<NodeBts> result2 = res.get(1).getFirst();
        result2.setConverter(NodeBts.class);
        assertEquals(1, result2.get().getWeekday());
    }
}
