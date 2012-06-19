package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.io.InputStream;
import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.MobilityConfiguration;

/**
 *
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Message>, LongWritable,
            TypedProtobufWritable<Int>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, TypedProtobufWritable<Message>,
                LongWritable, TypedProtobufWritable<Int>>(
                        new RepbtsFilterNumCommsReducer());
        InputStream configInput = MobilityConfiguration.class.getResource(
                "/mobility.properties").openStream();
        MobilityConfiguration conf = new MobilityConfiguration();
        conf.load(configInput);
        this.driver.setConfiguration(conf);
    }

    @Test
    public void testBelowMinThresolds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> cdr = new TypedProtobufWritable<Message>(
                Cdr.getDefaultInstance());
        final TypedProtobufWritable<Message> nodeBtsDay = new TypedProtobufWritable<Message>(
                NodeBtsDayUtil.create(57L, 35L, 5, 198));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay))
                .runTest();
    }
    
    @Test
    public void testBetweenThresholds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> cdr = new TypedProtobufWritable<Message>(
                Cdr.getDefaultInstance());
        final TypedProtobufWritable<Message> nodeBtsDay = new TypedProtobufWritable<Message>(
                NodeBtsDayUtil.create(57L, 35L, 5, 198));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay, cdr, cdr))
                .withOutput(key, TypedProtobufWritable.create(198))
                .runTest();
    }

    @Test
    public void testAboveMaxThresholds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final TypedProtobufWritable<Message> cdr = new TypedProtobufWritable<Message>(
                Cdr.getDefaultInstance());
        final TypedProtobufWritable<Message> nodeBtsDay = new TypedProtobufWritable<Message>(
                NodeBtsDayUtil.create(57L, 35L, 5, 4998));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay, cdr, cdr))
                .runTest();
    }
}
