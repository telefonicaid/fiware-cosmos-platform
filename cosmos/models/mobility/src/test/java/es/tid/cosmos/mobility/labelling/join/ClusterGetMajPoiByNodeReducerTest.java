package es.tid.cosmos.mobility.labelling.join;

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
public class ClusterGetMajPoiByNodeReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new ClusterGetMajPoiByNodeReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(34L, 76L));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(52L, 101L));
        final ProtobufWritable<MobData> value3 = MobDataUtil.createAndWrap(
                TwoIntUtil.create(107L, 65));
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                52L);
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }
}
