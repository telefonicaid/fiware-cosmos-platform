package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;

/**
 *
 * @author dmicol
 */
public class AdjCountIndexesReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Int64>, LongWritable,
            TypedProtobufWritable<Null>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Int64>,
                LongWritable, TypedProtobufWritable<Null>>(
                        new AdjCountIndexesReducer());
    }

    @Test
    public void testReduce() {
        this.driver
                .withInput(new LongWritable(3L),
                           asList(TypedProtobufWritable.create(5L),
                                  TypedProtobufWritable.create(10L),
                                  TypedProtobufWritable.create(7L),
                                  TypedProtobufWritable.create(0L)))
                .withOutput(new LongWritable(22L),
                            new TypedProtobufWritable<Null>(Null.getDefaultInstance()))
                .runTest();
    }
}
