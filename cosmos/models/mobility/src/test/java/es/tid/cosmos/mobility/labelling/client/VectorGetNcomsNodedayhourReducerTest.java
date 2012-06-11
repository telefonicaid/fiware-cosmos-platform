package es.tid.cosmos.mobility.labelling.client;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorGetNcomsNodedayhourReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBts>, MobilityWritable<Null>,
            ProtobufWritable<NodeBts>, MobilityWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                MobilityWritable<Null>, ProtobufWritable<NodeBts>,
                MobilityWritable<TwoInt>>(
                        new VectorGetNcomsNodedayhourReducer());
    }

    @Test
    public void testGetNumberOfComsPerNodeAndDailyhour() {
        final ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(1, 2,
                                                                        3, 4);
        final MobilityWritable<Null> value = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        final ProtobufWritable<NodeBts> outKey = NodeBtsUtil.createAndWrap(
                1, 2, 3, 0);
        final MobilityWritable<TwoInt> outValue = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(4, 3));
        this.driver
                .withInput(key, asList(value, value, value))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
