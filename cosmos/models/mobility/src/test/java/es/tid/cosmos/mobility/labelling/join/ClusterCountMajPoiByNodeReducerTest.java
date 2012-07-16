package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
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
public class ClusterCountMajPoiByNodeReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>,
            LongWritable, TypedProtobufWritable<TwoInt>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Null>, LongWritable, TypedProtobufWritable<TwoInt>>(
                        new ClusterCountMajPoiByNodeReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Null> value = new TypedProtobufWritable<Null>(
                Null.getDefaultInstance());
        final TypedProtobufWritable<TwoInt> outValue = new TypedProtobufWritable<TwoInt>(
                TwoIntUtil.create(32L, 3L));
        this.driver
                .withInput(key, asList(value, value, value))
                .withOutput(new LongWritable(57L), outValue)
                .runTest();
    }
}
