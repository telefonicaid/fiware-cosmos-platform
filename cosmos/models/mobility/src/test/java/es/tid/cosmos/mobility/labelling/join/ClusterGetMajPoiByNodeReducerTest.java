package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterGetMajPoiByNodeReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<TwoInt>, LongWritable,
            MobilityWritable<Int64>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<TwoInt>,
                LongWritable, MobilityWritable<Int64>>(
                        new ClusterGetMajPoiByNodeReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<TwoInt> value1 = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(34L, 76L));
        final MobilityWritable<TwoInt> value2 = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(52L, 101L));
        final MobilityWritable<TwoInt> value3 = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(107L, 65));
        final MobilityWritable<Int64> outValue = MobilityWritable.create(52L);
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }
}
