package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ExportBtsCounterToTextByTwoIntReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<BtsCounter>,
            NullWritable, Text> driver;
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<BtsCounter>, NullWritable, Text>(
                        new ExportBtsCounterToTextByTwoIntReducer());
    }
    
    @Test
    public void testSetInputId() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final MobilityWritable<BtsCounter> value = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(1, 2, 3, 4));
        final Text outValue = new Text("57|32|1|2|3|4");
        this.driver
                .withInput(key, asList(value))
                .withOutput(NullWritable.get(), outValue)
                .runTest();
    }
}
