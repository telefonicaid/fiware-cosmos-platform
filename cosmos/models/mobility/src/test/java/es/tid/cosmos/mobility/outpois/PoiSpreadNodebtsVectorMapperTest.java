package es.tid.cosmos.mobility.outpois;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiSpreadNodebtsVectorMapperTest {
    private MapDriver<ProtobufWritable<NodeBts>, MobilityWritable<ClusterVector>,
            ProtobufWritable<TwoInt>, MobilityWritable<ClusterVector>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<NodeBts>,
                MobilityWritable<ClusterVector>, ProtobufWritable<TwoInt>,
                MobilityWritable<ClusterVector>>(
                        new PoiSpreadNodebtsVectorMapper());
    }
    
    @Test
    public void testMap() {
        final ProtobufWritable<NodeBts> inputKey = NodeBtsUtil.createAndWrap(
                1L, 2, 3, 4);
        final ProtobufWritable<TwoInt> outputKey = TwoIntUtil.createAndWrap(
                1L, 2L);
        final MobilityWritable<ClusterVector> value = new MobilityWritable<ClusterVector>(
                ClusterVector.getDefaultInstance());
        this.driver
                .withInput(inputKey, value)
                .withOutput(outputKey, value)
                .runTest();
    }
}
