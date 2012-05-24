package es.tid.cosmos.mobility.labelling.clientbts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientbtsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
                ProtobufWritable<MobData>>(new VectorFiltClientbtsReducer());
    }

    @Test
    public void testSomeMethod() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        MobData value1 = MobDataUtil.create(BtsCounterUtil.create(1, 2, 3, 4));
        value1 = MobDataUtil.setInputId(value1, 0);
        MobData value2 = MobDataUtil.create(BtsCounterUtil.create(5, 6, 7, 8));
        value2 = MobDataUtil.setInputId(value2, 1);
        MobData value3 = MobDataUtil.create(BtsCounterUtil.create(1, 2, 3, 4));
        value3 = MobDataUtil.setInputId(value3, 1);
        ProtobufWritable<NodeBts> outKey = NodeBtsUtil.createAndWrap(57, 32,
                                                                     2, 0);
        ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                TwoIntUtil.create(3, 4));
        this.driver
                .withInput(key, asList(MobDataUtil.wrap(value1),
                                       MobDataUtil.wrap(value2),
                                       MobDataUtil.wrap(value3)))
                .withOutput(outKey, outValue)
                .withOutput(outKey, outValue)
                .runTest();
    }
}
