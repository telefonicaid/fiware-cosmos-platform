package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 *
 * @author dmicol
 */
public class ItinGetVectorReducerTest extends ConfiguredTest {
    private ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ItinPercMove>, ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ClusterVector>> instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ItinPercMove>, ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ClusterVector>>(new ItinGetVectorReducer());
        this.instance.setConfiguration(this.getConf());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(
                1, 2, 3, 4, 5);
        final TypedProtobufWritable<ItinPercMove> value1 =
                new TypedProtobufWritable<ItinPercMove>(
                        ItinPercMoveUtil.create(0, 7, 8.9D));
        final TypedProtobufWritable<ItinPercMove> value2 =
                new TypedProtobufWritable<ItinPercMove>(
                        ItinPercMoveUtil.create(6, 11, 12.13D));
        List<Pair<ProtobufWritable<ItinRange>,
                  TypedProtobufWritable<ClusterVector>>> results = this.instance
                .withInput(key, Arrays.asList(value1, value2))
                .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        assertEquals(key, results.get(0).getFirst());
        final TypedProtobufWritable<ClusterVector> outValue =
                results.get(0).getSecond();
        final ClusterVector clusterVector = outValue.get();
        assertEquals(168, clusterVector.getComsCount());
        assertEquals(8.9D, clusterVector.getComs(151), 0.0D);
        assertEquals(12.13D, clusterVector.getComs(131), 0.0D);
    }
}
