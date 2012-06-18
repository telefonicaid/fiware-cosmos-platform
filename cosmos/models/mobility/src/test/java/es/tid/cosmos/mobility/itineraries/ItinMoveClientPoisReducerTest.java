package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.TimeUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;

/**
 *
 * @author dmicol
 */
public class ItinMoveClientPoisReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<ItinTime>,
            LongWritable, TypedProtobufWritable<ItinMovement>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<ItinTime>, LongWritable,
                TypedProtobufWritable<ItinMovement>>(
                        new ItinMoveClientPoisReducer());
    }

    @Test
    public void shouldNotChangeBts() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final Date srcDate = DateUtil.create(2012, 05, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 05, 28, 1);
        final Time srcTime = TimeUtil.create(18, 01, 05);
        final Time tgtTime = TimeUtil.create(15, 35, 27);
        final TypedProtobufWritable<ItinTime> value1 =
                new TypedProtobufWritable<ItinTime>(ItinTimeUtil.create(
                        tgtDate, tgtTime, 3L));
        final TypedProtobufWritable<ItinTime> value2 =
                new TypedProtobufWritable<ItinTime>(ItinTimeUtil.create(
                        srcDate, srcTime, 3L));
        List<Pair<LongWritable, TypedProtobufWritable<ItinMovement>>> results =
                this.instance
                        .withInput(key, Arrays.asList(value1, value2))
                        .run();
        assertNotNull(results);
        assertEquals(0, results.size());
    }

    @Test
    public void shouldProduceOutput() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final Date srcDate = DateUtil.create(2012, 05, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 05, 27, 0);
        final Time srcTime = TimeUtil.create(15, 35, 27);
        final Time tgtTime = TimeUtil.create(18, 01, 05);
        final ItinTime time1 = ItinTimeUtil.create(tgtDate, tgtTime, 3L);
        final ItinTime time2 = ItinTimeUtil.create(srcDate, srcTime, 9L);
        final TypedProtobufWritable<ItinTime> value1 =
                new TypedProtobufWritable<ItinTime>(time1);
        final TypedProtobufWritable<ItinTime> value2 =
                new TypedProtobufWritable<ItinTime>(time2);
        final TypedProtobufWritable<ItinMovement> outValue =
                new TypedProtobufWritable<ItinMovement>(ItinMovementUtil.create(
                        time2, time1));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
