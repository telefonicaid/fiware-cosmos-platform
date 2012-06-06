package es.tid.cosmos.mobility.aggregatedmatrix.group;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.MatrixRangeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class MatrixCountRangesReducerTest {
    private ReduceDriver<ProtobufWritable<MatrixRange>,
            ProtobufWritable<MobData>, ProtobufWritable<MatrixRange>,
            ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<MatrixRange>,
                ProtobufWritable<MobData>, ProtobufWritable<MatrixRange>,
                ProtobufWritable<MobData>>(new MatrixCountRangesReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<MatrixRange> key = MatrixRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                5.7D);
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                8.8D);
        final ProtobufWritable<MatrixRange> outKey =
                MatrixRangeUtil.createAndWrap(1, 2, 3, 0, 0);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                ItinPercMoveUtil.create(4, 5, 14.5D));
        this.instance
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
