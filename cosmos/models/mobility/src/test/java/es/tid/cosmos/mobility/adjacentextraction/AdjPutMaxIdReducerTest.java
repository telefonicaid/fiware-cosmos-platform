package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class AdjPutMaxIdReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new AdjPutMaxIdReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = 
                MobDataUtil.createAndWrap(TwoIntUtil.create(1L, 3L));
        final ProtobufWritable<MobData> value2 =
                MobDataUtil.createAndWrap(TwoIntUtil.create(2L, 10L));
        final ProtobufWritable<MobData> value3 =
                MobDataUtil.createAndWrap(TwoIntUtil.create(3L, 1L));
        final ProtobufWritable<MobData> value4 =
                MobDataUtil.createAndWrap(TwoIntUtil.create(4L, 7L));
        this.driver
                .withInput(key, asList(value1, value2, value3, value4))
                .withOutput(key, MobDataUtil.createAndWrap(
                        TwoIntUtil.create(4L, 10l)))
                .runTest();
    }
}
