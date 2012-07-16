package es.tid.cosmos.mobility.outpois;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoivectorPoiReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Cluster>>(new PoiJoinPoivectorPoiReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ClusterVector clusterVector = ClusterVector.newBuilder()
                .addAllComs(asList(0.5D, 3.2D, 1.7D))
                .build();
        final Poi poi1 = PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                                        13, 14, 15, 16, 17);
        final Poi poi2 = PoiUtil.create(10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
                                        110, 120, 130, 140, 150, 160, 170);
        final ProtobufWritable<TwoInt> outKey1 = TwoIntUtil.createAndWrap(57L,
                                                                          1L);
        final Cluster outValue1 = ClusterUtil.create(12, 13, 14, 0.0D, 0.0D,
                                                     clusterVector);
        final ProtobufWritable<TwoInt> outKey2 = TwoIntUtil.createAndWrap(57L,
                                                                          10L);
        final Cluster outValue2 = ClusterUtil.create(120, 130, 140, 0.0D, 0.0D,
                                                     clusterVector);
        this.driver
                .withInput(key, asList(new TypedProtobufWritable<Message>(poi1),
                        new TypedProtobufWritable<Message>(clusterVector),
                        new TypedProtobufWritable<Message>(poi2)))
                .withOutput(outKey1, new TypedProtobufWritable<Cluster>(outValue1))
                .withOutput(outKey2, new TypedProtobufWritable<Cluster>(outValue2))
                .runTest();
    }
}
