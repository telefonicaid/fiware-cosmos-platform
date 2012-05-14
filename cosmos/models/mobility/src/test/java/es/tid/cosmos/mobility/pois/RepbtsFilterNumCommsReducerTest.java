package es.tid.cosmos.mobility.pois;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new RepbtsFilterNumCommsReducer());
    }

    @Test
    public void testBelowMinThresolds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> cdr = MobDataUtil.createAndWrap(
                Cdr.getDefaultInstance());
        final ProtobufWritable<MobData> nodeBtsDay = MobDataUtil.createAndWrap(
                NodeBtsDayUtil.create(57L, 35L, 5, 198));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay))
                .runTest();
    }
    
    @Test
    public void testBetweenThresholds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> cdr = MobDataUtil.createAndWrap(
                Cdr.getDefaultInstance());
        final ProtobufWritable<MobData> nodeBtsDay = MobDataUtil.createAndWrap(
                NodeBtsDayUtil.create(57L, 35L, 5, 198));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay, cdr, cdr))
                .withOutput(key, MobDataUtil.createAndWrap(198))
                .runTest();
    }

    @Test
    public void testAboveMaxThresholds() throws Exception {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> cdr = MobDataUtil.createAndWrap(
                Cdr.getDefaultInstance());
        final ProtobufWritable<MobData> nodeBtsDay = MobDataUtil.createAndWrap(
                NodeBtsDayUtil.create(57L, 35L, 5, 4998));
        this.driver
                .withInput(key, asList(cdr, nodeBtsDay, cdr, cdr))
                .runTest();
    }
}
