package es.tid.cosmos.mobility.labelling.client;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorGetNcomsNodedayhourReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
                ProtobufWritable<MobData>>(
                        new VectorGetNcomsNodedayhourReducer());
    }

    @Test
    public void testSomeMethod() {
        final ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(1, 2,
                                                                        3, 4);
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                NullWritable.get());
        final ProtobufWritable<NodeBts> outKey = NodeBtsUtil.createAndWrap(
                1, 2, 3, 0);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                TwoIntUtil.create(4, 3));
        this.driver
                .withInput(key, asList(value, value, value))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
