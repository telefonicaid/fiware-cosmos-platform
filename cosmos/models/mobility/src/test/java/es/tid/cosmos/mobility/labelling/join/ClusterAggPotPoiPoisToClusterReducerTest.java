package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToClusterReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<Message>,
        ProtobufWritable<TwoInt>, MobilityWritable<Cluster>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<Message>, ProtobufWritable<TwoInt>,
                MobilityWritable<Cluster>>(
                        new ClusterAggPotPoiPoisToClusterReducer());
    }

    @Test
    public void shoulChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                ClusterUtil.create(5, 6, 1, 7, 8,
                                   ClusterVector.getDefaultInstance()));
        // This value is what makes the reducer change the confidence
        final MobilityWritable<Message> value3 = new MobilityWritable<Message>(
                Null.getDefaultInstance());
        final MobilityWritable<Cluster> outValue = new MobilityWritable<Cluster>(
                ClusterUtil.create(1, 2, 1, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }
    
    @Test
    public void shouldNotChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                ClusterUtil.create(5, 6, 1, 7, 8,
                                   ClusterVector.getDefaultInstance()));
        final MobilityWritable<Cluster> outValue = new MobilityWritable<Cluster>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
