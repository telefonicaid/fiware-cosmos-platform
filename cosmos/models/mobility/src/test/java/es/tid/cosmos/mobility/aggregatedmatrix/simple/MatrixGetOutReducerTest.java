package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class MatrixGetOutReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>,
            MobilityWritable<ClusterVector>, NullWritable, Text> instance;

    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<ClusterVector>, NullWritable, Text>(
                        new MatrixGetOutReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L,
                                                                      102L);
        final MobilityWritable<ClusterVector> value = new MobilityWritable<ClusterVector>(
                ClusterVector.getDefaultInstance());
        List<Pair<NullWritable, Text>> results = this.instance
                .withInput(key, asList(value, value))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        assertEquals(NullWritable.get(), results.get(0).getFirst());
        assertTrue(results.get(0).getSecond().toString().startsWith("57|102"));
        assertEquals(NullWritable.get(), results.get(1).getFirst());
        assertTrue(results.get(1).getSecond().toString().startsWith("57|102"));
    }
}
