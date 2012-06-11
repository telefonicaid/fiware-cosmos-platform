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

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorSumGroupcommsReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBts>, MobilityWritable<Int>,
            ProtobufWritable<TwoInt>, MobilityWritable<BtsCounter>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                MobilityWritable<Int>, ProtobufWritable<TwoInt>,
                MobilityWritable<BtsCounter>>(new VectorSumGroupcommsReducer());
    }

    @Test
    public void test() throws IOException {
        ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(2342L, 1231,
                                                                  4, 2);
        List<Pair<ProtobufWritable<TwoInt>, MobilityWritable<BtsCounter>>> res =
                this.driver
                .withInput(key, asList(MobilityWritable.create(2),
                                       MobilityWritable.create(8),
                                       MobilityWritable.create(5)))
                .run();
        assertEquals(1, res.size());
        ProtobufWritable<TwoInt> outKey = res.get(0).getFirst();
        outKey.setConverter(TwoInt.class);
        assertNotNull(outKey.get());
        MobilityWritable<BtsCounter> outValue = res.get(0).getSecond();
        assertNotNull(outValue.get());
        assertEquals(15, outValue.get().getCount());
    }
}
