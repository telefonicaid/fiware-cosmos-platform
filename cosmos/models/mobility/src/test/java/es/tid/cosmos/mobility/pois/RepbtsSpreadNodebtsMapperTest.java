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

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapperTest {
    private MapDriver<LongWritable, MobilityWritable<NodeMxCounter>,
            ProtobufWritable<NodeBtsDay>, MobilityWritable<Int>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable,
                MobilityWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
                MobilityWritable<Int>>(new RepbtsSpreadNodebtsMapper());
    }
    
    @Test
    public void testMap() throws Exception {
        List<BtsCounter> bcList = new ArrayList<BtsCounter>();
        bcList.add(BtsCounterUtil.create(3L, 3, 4, 6));
        bcList.add(BtsCounterUtil.create(10L, 1, 9, 5));
        MobilityWritable<NodeMxCounter> counter = new MobilityWritable<NodeMxCounter>(
                NodeMxCounterUtil.create(bcList, 50, 70));
        List<Pair<ProtobufWritable<NodeBtsDay>, MobilityWritable<Int>>>
                results = this.driver
                        .withInput(new LongWritable(1L), counter)
                        .run();
        assertEquals(2, results.size());
        MobilityWritable<Int> value1 = results.get(0).getSecond();
        assertEquals(6, value1.get().getNum());
        MobilityWritable<Int> value2 = results.get(1).getSecond();
        assertEquals(5, value2.get().getNum());
    }
}
