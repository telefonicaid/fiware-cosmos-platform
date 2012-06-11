package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjJoinPairbtsAdjbtsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<InputIdRecord>,
            LongWritable, MobilityWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<InputIdRecord>, LongWritable,
                MobilityWritable<TwoInt>>(new AdjJoinPairbtsAdjbtsReducer());
    }

    @Test
    public void testWithOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        TwoInt value1 = TwoIntUtil.create(57L, 32L);
        MobilityWritable<InputIdRecord> record1 = new MobilityWritable<InputIdRecord>(
                InputIdRecord.newBuilder()
                             .setInputId(0)
                             .setMessageBytes(value1.toByteString())
                             .build());
        TwoInt value2 = TwoIntUtil.create(157L, 132L);
        MobilityWritable<InputIdRecord> record2 = new MobilityWritable<InputIdRecord>(
                InputIdRecord.newBuilder()
                             .setInputId(1)
                             .setMessageBytes(value2.toByteString())
                             .build());
        final LongWritable outKey = new LongWritable(57L);
        final MobilityWritable<TwoInt> outValue = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(57L, 32L));
        this.driver
                .withInput(key, asList(record1, record2))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void testNoOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        TwoInt value1 = TwoIntUtil.create(57L, 32L);
        MobilityWritable<InputIdRecord> record1 = new MobilityWritable<InputIdRecord>(
                InputIdRecord.newBuilder()
                             .setInputId(0)
                             .setMessageBytes(value1.toByteString())
                             .build());
        this.driver
                .withInput(key, asList(record1))
                .runTest();
    }
}
