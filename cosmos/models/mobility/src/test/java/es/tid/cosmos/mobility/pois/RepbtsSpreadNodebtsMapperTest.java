package es.tid.cosmos.mobility.pois;

import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapperTest {

    private MapDriver<LongWritable, TypedProtobufWritable<NodeMxCounter>,
            ProtobufWritable<NodeBtsDay>, TypedProtobufWritable<Int>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable,
                TypedProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
                TypedProtobufWritable<Int>>(new RepbtsSpreadNodebtsMapper());
    }

    @Test
    public void testMap() throws Exception {
        List<BtsCounter> bcList = new ArrayList<BtsCounter>();
        bcList.add(BtsCounterUtil.create(3L, 3, 4, 6));
        bcList.add(BtsCounterUtil.create(10L, 1, 9, 5));
        TypedProtobufWritable<NodeMxCounter> counter =
                new TypedProtobufWritable<NodeMxCounter>(
                NodeMxCounterUtil.create(bcList));
        List<Pair<ProtobufWritable<NodeBtsDay>, TypedProtobufWritable<Int>>>
                results = this.driver
                        .withInput(new LongWritable(1L), counter)
                        .run();
        assertEquals(2, results.size());
        TypedProtobufWritable<Int> value1 = results.get(0).getSecond();
        assertEquals(6, value1.get().getValue());
        TypedProtobufWritable<Int> value2 = results.get(1).getSecond();
        assertEquals(5, value2.get().getValue());
    }
}
