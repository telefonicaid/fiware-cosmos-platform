package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;

/**
 *
 * @author dmicol
 */
public class RepbtsJoinDistCommsReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>, LongWritable,
            MobilityWritable<BtsCounter>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                LongWritable, MobilityWritable<BtsCounter>>(
                        new RepbtsJoinDistCommsReducer());
    }

    @Test
    public void testReduce() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> nodeBtsDay1 = new MobilityWritable<Message>(
                NodeBtsDayUtil.create(1L, 2L, 3, 4));
        final MobilityWritable<Message> nodeBtsDay2 = new MobilityWritable<Message>(
                NodeBtsDayUtil.create(5L, 6L, 7, 8));
        final MobilityWritable<Message> int1 = new MobilityWritable<Message>(
                Int.newBuilder().setNum(37).build());
        final MobilityWritable<Message> int2 = new MobilityWritable<Message>(
                Int.newBuilder().setNum(132).build());
        final MobilityWritable<BtsCounter> output1 = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(2L, 0, 4, 10));
        final MobilityWritable<BtsCounter> output2 = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(6L, 0, 8, 21));
        final MobilityWritable<BtsCounter> output3 = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(2L, 0, 4, 3));
        final MobilityWritable<BtsCounter> output4 = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(6L, 0, 8, 6));
        this.driver
                .withInput(key, asList(int1, nodeBtsDay1, nodeBtsDay2, int2))
                .withOutput(key, output1)
                .withOutput(key, output2)
                .withOutput(key, output3)
                .withOutput(key, output4)
                .runTest();
    }
}
