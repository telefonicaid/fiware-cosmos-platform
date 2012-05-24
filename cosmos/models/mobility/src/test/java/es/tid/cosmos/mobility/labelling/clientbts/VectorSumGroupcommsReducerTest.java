package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorSumGroupcommsReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(new VectorSumGroupcommsReducer());
    }

    @Test
    public void test() throws IOException {
        ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(2342L, 1231,
                                                                  4, 2);
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>> res =
                this.driver
                .withInput(key, asList(MobDataUtil.createAndWrap(2),
                                       MobDataUtil.createAndWrap(8),
                                       MobDataUtil.createAndWrap(5)))
                .run();
        assertEquals(1, res.size());
        ProtobufWritable<TwoInt> outKey = res.get(0).getFirst();
        outKey.setConverter(TwoInt.class);
        assertNotNull(outKey.get());
        ProtobufWritable<MobData> outValue = res.get(0).getSecond();
        outValue.setConverter(MobData.class);
        assertNotNull(outValue.get());
        assertEquals(15, outValue.get().getBtsCounter().getCount());
    }
}
