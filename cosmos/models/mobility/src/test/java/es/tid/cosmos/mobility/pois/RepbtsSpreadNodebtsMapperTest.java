package es.tid.cosmos.mobility.pois;

import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.pois.RepbtsSpreadNodebtsMapper;

/**
 *
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<NodeMxCounter>,
            ProtobufWritable<NodeBtsDay>, IntWritable> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable,
                ProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
                IntWritable>(new RepbtsSpreadNodebtsMapper());
    }
    
    @Test
    public void testMap() throws Exception {
        List<BtsCounter> bcList = new ArrayList<BtsCounter>();
        bcList.add(BtsCounterUtil.create(3L, 3, 4, 6));
        bcList.add(BtsCounterUtil.create(10L, 1, 9, 5));
        ProtobufWritable<NodeMxCounter> counter = NodeMxCounterUtil.
                createAndWrap(bcList, 50, 70);
        List<Pair<ProtobufWritable<NodeBtsDay>, IntWritable>> results =
                this.driver
                        .withInput(new LongWritable(1L), counter)
                        .run();
        assertEquals(2, results.size());
        assertEquals(6, results.get(0).getSecond().get());
        assertEquals(5, results.get(1).getSecond().get());
    }
}
