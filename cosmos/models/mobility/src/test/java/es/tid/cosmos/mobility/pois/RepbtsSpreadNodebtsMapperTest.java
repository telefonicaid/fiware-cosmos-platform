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
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 *
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBtsDay>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        //this.driver = new MapDriver<LongWritable,
        //        ProtobufWritable<MobData>, ProtobufWritable<NodeBtsDay>,
        //        ProtobufWritable<MobData>>(new RepbtsSpreadNodebtsMapper());
    }
    
    @Test
    public void testMap() throws Exception {
        List<BtsCounter> bcList = new ArrayList<BtsCounter>();
        bcList.add(BtsCounterUtil.create(3L, 3, 4, 6));
        bcList.add(BtsCounterUtil.create(10L, 1, 9, 5));
        ProtobufWritable<MobData> counter = MobDataUtil.createAndWrap(
                NodeMxCounterUtil.create(bcList, 50, 70));
        List<Pair<ProtobufWritable<NodeBtsDay>, ProtobufWritable<MobData>>>
                results = this.driver
                        .withInput(new LongWritable(1L), counter)
                        .run();
        assertEquals(2, results.size());
        ProtobufWritable<MobData> value1 = results.get(0).getSecond();
        value1.setConverter(MobData.class);
        assertEquals(6, value1.get().getInt());
        ProtobufWritable<MobData> value2 = results.get(1).getSecond();
        value2.setConverter(MobData.class);
        assertEquals(5, value2.get().getInt());
    }
}
