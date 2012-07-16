package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class MatrixGetVectorReducerTest {
    private ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ItinPercMove>, ProtobufWritable<TwoInt>,
            TypedProtobufWritable<ClusterVector>> instance;

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ItinPercMove>, ProtobufWritable<TwoInt>,
            TypedProtobufWritable<ClusterVector>>(new MatrixGetVectorReducer());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        final TypedProtobufWritable<ItinPercMove> value1 = new TypedProtobufWritable<ItinPercMove>(
                ItinPercMoveUtil.create(0, 7, 8.9D));
        final TypedProtobufWritable<ItinPercMove> value2 = new TypedProtobufWritable<ItinPercMove>(
                ItinPercMoveUtil.create(6, 11, 12.13D));
        List<Pair<ProtobufWritable<TwoInt>, TypedProtobufWritable<ClusterVector>>> res =
                this.instance
                        .withInput(key, asList(value1, value2))
                        .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        final ProtobufWritable<TwoInt> outKey = res.get(0).getFirst();
        outKey.setConverter(TwoInt.class);
        assertEquals(1, outKey.get().getNum1());
        assertEquals(2, outKey.get().getNum2());
        final TypedProtobufWritable<ClusterVector> outValue = res.get(0).getSecond();
        final ClusterVector clusterVector = outValue.get();
        assertEquals(168, clusterVector.getComsCount());
        assertEquals(8.9D, clusterVector.getComs(151), 0.0D);
        assertEquals(12.13D, clusterVector.getComs(131), 0.0D);
    }
}
