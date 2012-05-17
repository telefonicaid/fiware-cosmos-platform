package es.tid.cosmos.mobility.outpois;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiSpreadNodebtsVectorMapperTest {
    private MapDriver<ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<ProtobufWritable<NodeBts>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(new PoiSpreadNodebtsVectorMapper());
    }
    
    @Test
    public void testMap() {
        final ProtobufWritable<NodeBts> inputKey = NodeBtsUtil.createAndWrap(
                1L, 2, 3, 4);
        final ProtobufWritable<TwoInt> outputKey = TwoIntUtil.createAndWrap(
                1L, 2L);
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                ClusterVector.getDefaultInstance());
        this.driver
                .withInput(inputKey, value)
                .withOutput(outputKey, value)
                .runTest();
    }
}
