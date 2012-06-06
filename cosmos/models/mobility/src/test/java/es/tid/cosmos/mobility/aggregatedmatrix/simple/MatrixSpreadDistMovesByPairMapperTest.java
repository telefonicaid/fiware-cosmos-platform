package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class MatrixSpreadDistMovesByPairMapperTest {
    private MapDriver<ProtobufWritable<ItinRange>, ProtobufWritable<MobData>,
            ProtobufWritable<ItinRange>, ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<ItinRange>,
                ProtobufWritable<MobData>, ProtobufWritable<ItinRange>,
                ProtobufWritable<MobData>>(
                        new MatrixSpreadDistMovesByPairMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                10, 20, 30, 40, 50);
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(57L);
        final ProtobufWritable<ItinRange> outKey = ItinRangeUtil.createAndWrap(
                10, 20, 200, 40, 50);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}
