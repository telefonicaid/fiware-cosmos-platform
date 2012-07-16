package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiDeleteSechomeDuplicateReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Null>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Null>>(
                        new PoiDeleteSechomeDuplicateReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(30L, 40L);
        final TypedProtobufWritable<Null> value = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        this.driver
                .withInput(key, asList(value, value, value))
                .withOutput(key, value)
                .runTest();
    }
}
