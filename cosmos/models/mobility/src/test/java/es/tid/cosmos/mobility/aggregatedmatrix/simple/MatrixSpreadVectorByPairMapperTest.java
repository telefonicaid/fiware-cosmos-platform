package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class MatrixSpreadVectorByPairMapperTest {
    private MapDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ClusterVector>, ProtobufWritable<TwoInt>,
            TypedProtobufWritable<ClusterVector>> instance;

    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<ItinRange>,
                TypedProtobufWritable<ClusterVector>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<ClusterVector>>(
                        new MatrixSpreadVectorByPairMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        final TypedProtobufWritable<ClusterVector> value = new TypedProtobufWritable<ClusterVector>(
                ClusterVector.getDefaultInstance());
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(1, 2);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}