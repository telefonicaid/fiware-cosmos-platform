package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public class MatrixSpreadDistMovesByPairMapperTest {
    private MapDriver<ProtobufWritable<ItinRange>, MobilityWritable<Float64>,
            ProtobufWritable<ItinRange>, MobilityWritable<Float64>> instance;
    
    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<ItinRange>,
                MobilityWritable<Float64>, ProtobufWritable<ItinRange>,
                MobilityWritable<Float64>>(
                        new MatrixSpreadDistMovesByPairMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                10, 20, 30, 40, 50);
        final MobilityWritable<Float64> value = MobilityWritable.create(57.0D);
        final ProtobufWritable<ItinRange> outKey = ItinRangeUtil.createAndWrap(
                10, 20, 200, 40, 50);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}
