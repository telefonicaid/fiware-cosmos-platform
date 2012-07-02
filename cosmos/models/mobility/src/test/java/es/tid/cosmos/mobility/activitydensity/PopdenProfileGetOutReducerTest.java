package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PopdenProfileGetOutReducerTest extends ConfiguredTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<ClusterVector>,
            NullWritable, Text> instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<ClusterVector>, NullWritable, Text>(
                        new PopdenProfileGetOutReducer());
        this.instance.setConfiguration(this.getConf());
    }
    
    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 88L);
        final TypedProtobufWritable<ClusterVector> value = new TypedProtobufWritable<ClusterVector>(
                ClusterVector.getDefaultInstance());
        List<Pair<NullWritable, Text>> results = this.instance
                .withInput(key, Arrays.asList(value))
                .run();
        assertNotNull(results);
        assertEquals(1, results.size());
        assertEquals(NullWritable.get(), results.get(0).getFirst());
        assertTrue(results.get(0).getSecond().toString().startsWith("57|88"));
    }
}
