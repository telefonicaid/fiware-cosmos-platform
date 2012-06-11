package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiFilterSechomeAdjacentReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<InputIdRecord>,
            ProtobufWritable<TwoInt>, MobilityWritable<Null>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<InputIdRecord>, ProtobufWritable<TwoInt>,
                MobilityWritable<Null>>(new PoiFilterSechomeAdjacentReducer());
    }

    @Test
    public void testWithOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final MobilityWritable<Int64> value1 = MobilityWritable.create(17L);
        final MobilityWritable<InputIdRecord> record1 =
                new MobilityWritable<InputIdRecord>(InputIdRecord
                        .newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value1.get().toByteString())
                        .build());
        final ProtobufWritable<TwoInt> outKey1 = TwoIntUtil.createAndWrap(17L,
                                                                          32L);
        final MobilityWritable<Int64> value2 = MobilityWritable.create(88L);
        final MobilityWritable<InputIdRecord> record2 =
                new MobilityWritable<InputIdRecord>(InputIdRecord
                        .newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value2.get().toByteString())
                        .build());
        final ProtobufWritable<TwoInt> outKey2 = TwoIntUtil.createAndWrap(88L,
                                                                          32L);
        final MobilityWritable<Null> outValue = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(key, asList(record1, record2))
                .withOutput(outKey1, outValue)
                .withOutput(outKey2, outValue)
                .runTest();
    }
    
    @Test
    public void testEmptyOutput() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final MobilityWritable<Int64> value1 = MobilityWritable.create(17L);
        final MobilityWritable<InputIdRecord> record1 =
                new MobilityWritable<InputIdRecord>(InputIdRecord
                        .newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value1.get().toByteString())
                        .build());
        final MobilityWritable<Int64> value2 = MobilityWritable.create(0L);
        final MobilityWritable<InputIdRecord> record2 =
                new MobilityWritable<InputIdRecord>(InputIdRecord
                        .newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value2.get().toByteString())
                        .build());
        final MobilityWritable<Int64> value3 = MobilityWritable.create(88L);
        final MobilityWritable<InputIdRecord> record3 =
                new MobilityWritable<InputIdRecord>(InputIdRecord
                        .newBuilder()
                        .setInputId(1)
                        .setMessageBytes(value3.get().toByteString())
                        .build());
        this.driver
                .withInput(key, asList(record1, record2, record3))
                .runTest();
    }
}
