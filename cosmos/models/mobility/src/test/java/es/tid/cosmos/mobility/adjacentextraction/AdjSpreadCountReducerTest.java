package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjSpreadCountReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new AdjSpreadCountReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(100L, 200L));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(300L, 400L));
        this.driver
                .withInput(new LongWritable(57L), asList(value1, value2))
                .withOutput(new LongWritable(0L), MobDataUtil.createAndWrap(2))
                .runTest();
    }
}
