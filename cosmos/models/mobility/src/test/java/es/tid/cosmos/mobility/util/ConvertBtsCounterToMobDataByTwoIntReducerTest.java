package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class ConvertBtsCounterToMobDataByTwoIntReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<BtsCounter>, ProtobufWritable<TwoInt>,
                ProtobufWritable<MobData>>(
                        new ConvertBtsCounterToMobDataByTwoIntReducer());
    }
    
    @Test
    public void testSomeMethod() throws IOException {
        ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        BtsCounter btsCounter = BtsCounterUtil.create(1L, 2, 3, 4);
        ProtobufWritable<BtsCounter> value = BtsCounterUtil.wrap(btsCounter);
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>> out =
                this.driver
                        .withInput(key, asList(value))
                        .run();
        assertNotNull(out);
        assertEquals(1, out.size());
        assertEquals(key, out.get(0).getFirst());
        ProtobufWritable<MobData> outValue1 = out.get(0).getSecond();
        outValue1.setConverter(MobData.class);
        assertEquals(MobData.Type.BTS_COUNTER, outValue1.get().getType());
        assertEquals(btsCounter, outValue1.get().getBtsCounter());
    }
}
