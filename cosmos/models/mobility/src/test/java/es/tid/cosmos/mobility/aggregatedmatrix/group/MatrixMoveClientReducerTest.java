package es.tid.cosmos.mobility.aggregatedmatrix.group;

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
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;

/**
 * @author dmicol
 */
public class MatrixMoveClientReducerTest extends ConfiguredTest {

    private ReduceDriver<LongWritable, TypedProtobufWritable<MatrixTime>,
            LongWritable, TypedProtobufWritable<ItinMovement>> instance;

    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<LongWritable,
                TypedProtobufWritable<MatrixTime>, LongWritable,
                TypedProtobufWritable<ItinMovement>>(
                new MatrixMoveClientReducer());
        this.instance.setConfiguration(this.getConf());
    }

    @Test
    public void shouldNotChangeGroup() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final Date srcDate = DateUtil.create(2012, 5, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 5, 28, 1);
        final Time srcTime = TimeUtil.create(18, 1, 5);
        final Time tgtTime = TimeUtil.create(15, 35, 27);
        final TypedProtobufWritable<MatrixTime> value1 =
                new TypedProtobufWritable<MatrixTime>(MatrixTimeUtil.create(
                        tgtDate, tgtTime, 57, 9L));
        final TypedProtobufWritable<MatrixTime> value2 =
                new TypedProtobufWritable<MatrixTime>(MatrixTimeUtil.create(
                        srcDate, srcTime, 57, 3L));
        List<Pair<LongWritable, TypedProtobufWritable<ItinMovement>>> results =
                this.instance
                        .withInput(key, Arrays.asList(value1, value2))
                        .run();
        assertNotNull(results);
        assertEquals(0, results.size());
    }

    @Test
    public void shouldNotChangeBts() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final Date srcDate = DateUtil.create(2012, 5, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 5, 28, 1);
        final Time srcTime = TimeUtil.create(18, 1, 5);
        final Time tgtTime = TimeUtil.create(15, 35, 27);
        final TypedProtobufWritable<MatrixTime> value1 =
                new TypedProtobufWritable<MatrixTime>(MatrixTimeUtil.create(
                        tgtDate, tgtTime, 57, 3L));
        final TypedProtobufWritable<MatrixTime> value2 =
                new TypedProtobufWritable<MatrixTime>(MatrixTimeUtil.create(
                        srcDate, srcTime, 60, 3L));
        List<Pair<LongWritable, TypedProtobufWritable<ItinMovement>>> results =
                this.instance
                        .withInput(key, Arrays.asList(value1, value2))
                        .run();
        assertNotNull(results);
        assertEquals(0, results.size());
    }

    @Test
    public void shouldProduceOutput() {
        final LongWritable key = new LongWritable(57L);
        final Date srcDate = DateUtil.create(2012, 5, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 5, 27, 0);
        final Time srcTime = TimeUtil.create(15, 35, 27);
        final Time tgtTime = TimeUtil.create(18, 1, 5);
        final MatrixTime time1 = MatrixTimeUtil.create(tgtDate, tgtTime, 57, 3L);
        final MatrixTime time2 = MatrixTimeUtil.create(srcDate, srcTime, 60, 9L);
        final ItinTime itinTime1 = ItinTimeUtil.create(tgtDate, tgtTime, 57);
        final ItinTime itinTime2 = ItinTimeUtil.create(srcDate, srcTime, 60);
        final TypedProtobufWritable<MatrixTime> value1 =
                new TypedProtobufWritable<MatrixTime>(time1);
        final TypedProtobufWritable<MatrixTime> value2 =
                new TypedProtobufWritable<MatrixTime>(time2);
        final TypedProtobufWritable<ItinMovement> outValue =
                new TypedProtobufWritable<ItinMovement>(ItinMovementUtil.create(
                        itinTime2, itinTime1));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testDateLogic() {
        final LongWritable key = new LongWritable(57L);
        final Date srcDate = DateUtil.create(2012, 5, 27, 0);
        final Date tgtDate = DateUtil.create(2012, 5, 27, 0);
        final Time srcTime = TimeUtil.create(15, 35, 27);
        final Time tgtTime = TimeUtil.create(18, 1, 5);
        final MatrixTime time1 = MatrixTimeUtil.create(tgtDate, tgtTime, 57, 3L);
        final MatrixTime time2 = MatrixTimeUtil.create(srcDate, srcTime, 60, 9L);
        final ItinTime itinTime1 = ItinTimeUtil.create(tgtDate, tgtTime, 57);
        final ItinTime itinTime2 = ItinTimeUtil.create(srcDate, srcTime, 60);
        final TypedProtobufWritable<MatrixTime> value1 =
                new TypedProtobufWritable<MatrixTime>(time1);
        final TypedProtobufWritable<MatrixTime> value2 =
                new TypedProtobufWritable<MatrixTime>(time2);
        final TypedProtobufWritable<ItinMovement> outValue =
                new TypedProtobufWritable<ItinMovement>(ItinMovementUtil.create(
                        itinTime2, itinTime1));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
