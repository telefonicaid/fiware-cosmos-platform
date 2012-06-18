package es.tid.cosmos.mobility.itineraries;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public class ItinCountRangesReducerTest {
    private ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<Float64>, ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ItinPercMove>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<ItinRange>,
                TypedProtobufWritable<Float64>, ProtobufWritable<ItinRange>,
                TypedProtobufWritable<ItinPercMove>>(
                        new ItinCountRangesReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        final TypedProtobufWritable<Float64> value1 =
                TypedProtobufWritable.create(5.7D);
        final TypedProtobufWritable<Float64> value2 =
                TypedProtobufWritable.create(8.8D);
        final ProtobufWritable<ItinRange> outKey =
                ItinRangeUtil.createAndWrap(1, 2, 3, 0, 0);
        final TypedProtobufWritable<MobProtocol.ItinPercMove> outValue =
                new TypedProtobufWritable<MobProtocol.ItinPercMove>(
                        ItinPercMoveUtil.create(4, 5, 14.5D));
        this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
