package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import java.io.InputStream;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;

/**
 *
 * @author dmicol
 */
public class ItinGetItineraryReducerTest {
    private ReduceDriver<ProtobufWritable<ItinRange>,
            TypedProtobufWritable<ClusterVector>, LongWritable,
            TypedProtobufWritable<Itinerary>> instance;
    
    @Before
    public void setUp() throws IOException {
        this.instance = new ReduceDriver<ProtobufWritable<ItinRange>,
                TypedProtobufWritable<ClusterVector>, LongWritable,
                TypedProtobufWritable<Itinerary>>(new ItinGetItineraryReducer());
        InputStream configInput = MobilityConfiguration.class.getResource(
                "/mobility.properties").openStream();
        MobilityConfiguration conf = new MobilityConfiguration();
        conf.load(configInput);
        this.instance.setConfiguration(conf);
    }

    // TODO: add a test that actually produces results
    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<ItinRange> key = ItinRangeUtil.createAndWrap(1L,
                2L, 3, 4, 5);
        ClusterVector.Builder clusterVector = ClusterVector.newBuilder();
        for (int i = 0; i < 168; i++) {
            clusterVector.addComs(i);
        }
        final TypedProtobufWritable<ClusterVector> value =
                new TypedProtobufWritable<ClusterVector>(clusterVector.build());
        List<Pair<LongWritable, TypedProtobufWritable<Itinerary>>> results =
                this.instance
                        .withInput(key, Arrays.asList(value))
                        .run();
        assertNotNull(results);
        assertEquals(0, results.size());
    }
}
