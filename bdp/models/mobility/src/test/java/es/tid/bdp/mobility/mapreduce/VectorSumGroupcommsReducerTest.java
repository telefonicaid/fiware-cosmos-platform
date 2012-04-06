package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.mobility.data.MobProtocol.BtsCounter;
import es.tid.bdp.mobility.data.MobProtocol.NodeBts;
import es.tid.bdp.mobility.data.MobProtocol.TwoInt;
import es.tid.bdp.mobility.data.NodeBtsUtil;

/**
 *
 * @author dmicol
 */
public class VectorSumGroupcommsReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBts>, IntWritable,
            ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>, IntWritable,
                ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>>(
                        new VectorSumGroupcommsReducer());
    }

    @Test
    public void test() throws IOException {
        ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(2342L, 1231,
                                                                  4, 2);
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>>>
                results = this.driver
                .withInput(key, asList(new IntWritable(2),
                                       new IntWritable(8),
                                       new IntWritable(5)))
                .run();
        assertEquals(1, results.size());
        ProtobufWritable<TwoInt> outKey = results.get(0).getFirst();
        outKey.setConverter(TwoInt.class);
        assertNotNull(outKey.get());
        ProtobufWritable<BtsCounter> outValue = results.get(0).getSecond();
        outValue.setConverter(BtsCounter.class);
        assertNotNull(outValue.get());
        assertEquals(15, outValue.get().getCount());
    }
}
