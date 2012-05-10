package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToClusterReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(
                        new ClusterAggPotPoiPoisToClusterReducer());
    }

    @Test
    public void shoulChangeConfidence() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(5, 6, 1, 7, 8,
                                   ClusterVector.getDefaultInstance()));
        // This value is what makes the reducer change the confidence
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(
                NullWritable.get());
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
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
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                ClusterUtil.create(5, 6, 1, 7, 8,
                                   ClusterVector.getDefaultInstance()));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
