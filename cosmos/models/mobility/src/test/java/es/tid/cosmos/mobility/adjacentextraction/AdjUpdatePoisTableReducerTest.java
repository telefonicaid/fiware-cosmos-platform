package es.tid.cosmos.mobility.adjacentextraction;

import java.util.Arrays;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author ximo
 */
public class AdjUpdatePoisTableReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<InputIdRecord>,
            LongWritable, TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable,
            TypedProtobufWritable<InputIdRecord>, LongWritable,
            TypedProtobufWritable<TwoInt>>(new AdjUpdatePoisTableReducer());
    }

    @Test
    public void testReducer() {
        LongWritable key = new LongWritable(3L);
        TwoInt value1 = TwoInt.newBuilder().setNum1(1L).setNum2(2L).build();
        TypedProtobufWritable<InputIdRecord> record1 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value1.toByteString())
                        .build());
        TwoInt value2 = TwoInt.newBuilder().setNum1(3L).setNum2(4L).build();
        TypedProtobufWritable<InputIdRecord> record2 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value2.toByteString())
                        .build());
        TwoInt value3 = TwoInt.newBuilder().setNum1(-7L).setNum2(-46L).build();
        TypedProtobufWritable<InputIdRecord> record3 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(1)
                        .setMessageBytes(value3.toByteString())
                        .build());
        TwoInt value4 = TwoInt.newBuilder().setNum1(500L).setNum2(600L).build();
        TypedProtobufWritable<InputIdRecord> record4 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value4.toByteString())
                        .build());
        TwoInt value5 = TwoInt.newBuilder().setNum1(5L).setNum2(6L).build();
        TypedProtobufWritable<InputIdRecord> record5 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(1)
                        .setMessageBytes(value5.toByteString())
                        .build());
        TwoInt value6 = TwoInt.newBuilder().setNum1(6L).setNum2(50L).build();
        TypedProtobufWritable<InputIdRecord> record6 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value6.toByteString())
                        .build());

        TypedProtobufWritable<TwoInt> out1 = new TypedProtobufWritable<TwoInt>(
                TwoInt.newBuilder().setNum1(1L).setNum2(6L).build());
        TypedProtobufWritable<TwoInt> out2 = new TypedProtobufWritable<TwoInt>(
                TwoInt.newBuilder().setNum1(3L).setNum2(6L).build());
        TypedProtobufWritable<TwoInt> out4 = new TypedProtobufWritable<TwoInt>(
                TwoInt.newBuilder().setNum1(500L).setNum2(6L).build());
        TypedProtobufWritable<TwoInt> out6 = new TypedProtobufWritable<TwoInt>(
                TwoInt.newBuilder().setNum1(6L).setNum2(6L).build());
        this.driver
                .withInput(key, Arrays.asList(record1, record2, record3,
                                              record4, record5, record6))
                .withOutput(new LongWritable(6L), out1)
                .withOutput(new LongWritable(6L), out2)
                .withOutput(new LongWritable(6L), out4)
                .withOutput(new LongWritable(6L), out6)
                .runTest();
    }

    @Test
    public void testReducerNotInput1() {
        LongWritable key = new LongWritable(3L);
        TwoInt value1 = TwoInt.newBuilder().setNum1(1L).setNum2(2L).build();
        TypedProtobufWritable<InputIdRecord> record1 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value1.toByteString())
                        .build());
        TwoInt value2 = TwoInt.newBuilder().setNum1(3L).setNum2(4L).build();
        TypedProtobufWritable<InputIdRecord> record2 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value2.toByteString())
                        .build());
        TwoInt value4 = TwoInt.newBuilder().setNum1(500L).setNum2(600L).build();
        TypedProtobufWritable<InputIdRecord> record4 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value4.toByteString())
                        .build());
        TwoInt value6 = TwoInt.newBuilder().setNum1(6L).setNum2(50L).build();
        TypedProtobufWritable<InputIdRecord> record6 =
                new TypedProtobufWritable<InputIdRecord>(InputIdRecord.newBuilder()
                        .setInputId(0)
                        .setMessageBytes(value6.toByteString())
                        .build());

        this.driver
                .withInput(key, Arrays.asList(record1, record2,
                                              record4, record6))
                .withOutput(new LongWritable(3L),
                            new TypedProtobufWritable<TwoInt>(value1))
                .withOutput(new LongWritable(3L),
                            new TypedProtobufWritable<TwoInt>(value2))
                .withOutput(new LongWritable(3L),
                            new TypedProtobufWritable<TwoInt>(value4))
                .withOutput(new LongWritable(3L),
                            new TypedProtobufWritable<TwoInt>(value6))
                .runTest();
    }
}
