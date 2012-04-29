package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorFilterBtsReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<TwoInt>,
            ProtobufWritable<BtsCounter>, NullWritable> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<TwoInt>,
                ProtobufWritable<BtsCounter>, NullWritable>(
                        new VectorFilterBtsReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        final LongWritable key = new LongWritable(3L);
        final TwoInt value = TwoIntUtil.create(4L, 7L);
        final BtsCounter outKey = BtsCounterUtil.create(3L, 4, 7, 0);
        this.driver
                .withInput(key, asList(TwoIntUtil.wrap(value)))
                .withOutput(BtsCounterUtil.wrap(outKey), NullWritable.get())
                .runTest();
    }
}
