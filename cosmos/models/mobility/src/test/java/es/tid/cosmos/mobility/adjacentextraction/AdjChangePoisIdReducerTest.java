package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjChangePoisIdReducerTest {

    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>>(new AdjChangePoisIdReducer());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 88L);
        final TypedProtobufWritable<Message> value1 =
                new TypedProtobufWritable<Message>(PoiUtil.create(1, 2L, 3L, 4,
                        5, 6, 7D, 8, 9, 10, 11, 12, 13, 14, 15D, 16, 17));
        final TypedProtobufWritable<Message> value2 =
                new TypedProtobufWritable<Message>(PoiNewUtil.create(10, 20L,
                        30L, 40, 50));
        List<Pair<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>>> res =
                this.instance
                        .withInput(key, Arrays.asList(value1, value2))
                        .run();
        assertNotNull(res);
        assertEquals(1, res.size());
        assertEquals(key, res.get(0).getFirst());
        final TypedProtobufWritable<Poi> outValue = res.get(0).getSecond();
        assertEquals(10, outValue.get().getId());
    }
}
