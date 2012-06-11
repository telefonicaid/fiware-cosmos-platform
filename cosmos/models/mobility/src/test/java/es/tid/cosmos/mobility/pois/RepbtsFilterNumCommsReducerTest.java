package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.io.InputStream;
import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;

/**
 *
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>, LongWritable,
            MobilityWritable<Int>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                LongWritable, MobilityWritable<Int>>(
                        new RepbtsFilterNumCommsReducer());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.driver.setConfiguration(Config.load(configInput,
                this.driver.getConfiguration()));
    }

    @Test
    public void testBelowMinThresolds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> cdr = new MobilityWritable<Message>(
                Cdr.getDefaultInstance());
        final MobilityWritable<Message> nodeBtsDay = new MobilityWritable<Message>(
                NodeBtsDayUtil.create(57L, 35L, 5, 198));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay))
                .runTest();
    }
    
    @Test
    public void testBetweenThresholds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> cdr = new MobilityWritable<Message>(
                Cdr.getDefaultInstance());
        final MobilityWritable<Message> nodeBtsDay = new MobilityWritable<Message>(
                NodeBtsDayUtil.create(57L, 35L, 5, 198));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay, cdr, cdr))
                .withOutput(key, MobilityWritable.create(198))
                .runTest();
    }

    @Test
    public void testAboveMaxThresholds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> cdr = new MobilityWritable<Message>(
                Cdr.getDefaultInstance());
        final MobilityWritable<Message> nodeBtsDay = new MobilityWritable<Message>(
                NodeBtsDayUtil.create(57L, 35L, 5, 4998));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay, cdr, cdr))
                .runTest();
    }
}
