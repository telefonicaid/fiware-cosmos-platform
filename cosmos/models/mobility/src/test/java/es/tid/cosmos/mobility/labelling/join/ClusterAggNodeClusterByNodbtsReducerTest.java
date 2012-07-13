package es.tid.cosmos.mobility.labelling.join;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterAggNodeClusterByNodbtsReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>>(
                        new ClusterAggNodeClusterByNodbtsReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> value1 = new TypedProtobufWritable<Message>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final TypedProtobufWritable<Message> value2 = new TypedProtobufWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final TypedProtobufWritable<Poi> outValue = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 1, 2, 0, 4, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
