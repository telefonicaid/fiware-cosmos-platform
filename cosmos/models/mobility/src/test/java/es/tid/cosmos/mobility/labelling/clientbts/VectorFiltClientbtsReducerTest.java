package es.tid.cosmos.mobility.labelling.clientbts;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientbtsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<InputIdRecord>,
        ProtobufWritable<NodeBts>, MobilityWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<InputIdRecord>, ProtobufWritable<NodeBts>,
                MobilityWritable<TwoInt>>(new VectorFiltClientbtsReducer());
    }

    @Test
    public void testSomeMethod() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        BtsCounter value1 = BtsCounterUtil.create(1, 2, 3, 4);
        MobilityWritable<InputIdRecord> record1 =
                new MobilityWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value1.toByteString())
                        .build());
        BtsCounter value2 = BtsCounterUtil.create(5, 6, 7, 8);
        MobilityWritable<InputIdRecord> record2 =
                new MobilityWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(1)
                        .setMessageBytes(value2.toByteString())
                        .build());
        BtsCounter value3 = BtsCounterUtil.create(1, 2, 3, 4);
        MobilityWritable<InputIdRecord> record3 =
                new MobilityWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(1)
                        .setMessageBytes(value3.toByteString())
                        .build());
        ProtobufWritable<NodeBts> outKey = NodeBtsUtil.createAndWrap(57, 32,
                                                                     2, 0);
        MobilityWritable<TwoInt> outValue = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(3, 4));
        this.driver
                .withInput(key, asList(record1, record2, record3))
                .withOutput(outKey, outValue)
                .withOutput(outKey, outValue)
                .runTest();
    }
}
