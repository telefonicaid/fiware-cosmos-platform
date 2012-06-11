package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadCountReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<TwoInt>, LongWritable,
            MobilityWritable<Int>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<TwoInt>,
                LongWritable, MobilityWritable<Int>>(
                        new AdjSpreadCountReducer());
    }

    @Test
    public void testReduce() {
        final MobilityWritable<TwoInt> value1 = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(100L, 200L));
        final MobilityWritable<TwoInt> value2 = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(300L, 400L));
        this.driver
                .withInput(new LongWritable(57L), asList(value1, value2))
                .withOutput(new LongWritable(0L), MobilityWritable.create(2))
                .runTest();
    }
}
