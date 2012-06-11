package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterCountMajPoiByNodeReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<Null>,
            LongWritable, MobilityWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<Null>, LongWritable, MobilityWritable<TwoInt>>(
                        new ClusterCountMajPoiByNodeReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final MobilityWritable<Null> value = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        final MobilityWritable<TwoInt> outValue = new MobilityWritable<TwoInt>(
                TwoIntUtil.create(32L, 3L));
        this.driver
                .withInput(key, asList(value, value, value))
                .withOutput(new LongWritable(57L), outValue)
                .runTest();
    }
}
