package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.when;
import org.powermock.api.mockito.PowerMockito;
import org.powermock.core.classloader.annotations.PrepareForTest;
import org.powermock.modules.junit4.PowerMockRunner;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector.Builder;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 *
 * @author ximo
 */
@RunWith(PowerMockRunner.class)
@PrepareForTest(CentroidsCatalogue.class)
public class ClusterClientGetMinDistanceReducerTest {
    
    private static final int VECTOR_LENGTH = 24;    
    
    private List<Cluster> centroids;
    private ReduceDriver<ProtobufWritable<NodeBts>,
            TypedProtobufWritable<ClusterVector>, LongWritable,
            TypedProtobufWritable<Cluster>> driver;
    
    @Before
    public void setUp() throws IOException {
        Builder vectorBuilder = ClusterVector.newBuilder();
        for (int i = 0; i < VECTOR_LENGTH; i++) {
            vectorBuilder.addComs(i);
        }
        final ClusterVector vector = vectorBuilder.build();
        this.centroids = new LinkedList<Cluster>();
        this.centroids.add(ClusterUtil.create(1, 2, 3, 4.2, 5.3, vector));
        this.centroids.add(ClusterUtil.create(6, 7, 8, 9.2, 10.3, vector));
        PowerMockito.mockStatic(CentroidsCatalogue.class);
        when(CentroidsCatalogue.load(any(Path.class),
                                     any(MobilityConfiguration.class)))
                .thenReturn(centroids);
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                TypedProtobufWritable<ClusterVector>, LongWritable,
                TypedProtobufWritable<Cluster>>(
                        new ClusterClientGetMinDistanceReducer());
        this.driver.setConfiguration(new MobilityConfiguration());
        this.driver.getConfiguration().set("centroids", "/home/test");
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(
                11L, 12L, 13, 14);
        List<TypedProtobufWritable<ClusterVector>> vectors =
                new LinkedList<TypedProtobufWritable<ClusterVector>>();
        for(int i = 0; i < 5; i++) {
            Builder vectorBuilder = ClusterVector.newBuilder();            
            for (int j = 0; j < VECTOR_LENGTH; j++) {
                vectorBuilder.addComs(VECTOR_LENGTH*j + i);
            }
            vectors.add(new TypedProtobufWritable<ClusterVector>(
                    vectorBuilder.build()));
        }
        final long outKey = key.get().getUserId();
        List<Pair<LongWritable, TypedProtobufWritable<Cluster>>> results =
                this.driver.withInput(key, vectors).run();
        assertEquals(results.size(), 5);
        for(Pair<LongWritable, TypedProtobufWritable<Cluster>> result : results) {
            assertEquals(result.getFirst().get(), outKey);
        }
    }
}
