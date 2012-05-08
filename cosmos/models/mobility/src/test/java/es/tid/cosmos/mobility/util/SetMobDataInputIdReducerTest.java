package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Test;
import org.junit.Before;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import org.apache.hadoop.io.LongWritable;

/**
 *
 * @author dmicol
 */
public class SetMobDataInputIdReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new SetMobDataInputIdReducer());
    }
    
    @Test(expected=IllegalArgumentException.class)
    public void testNoInputId() {
        this.driver.runTest();
    }
    
    @Test
    public void testSetInputId() throws IOException {
        Configuration conf = this.driver.getConfiguration();
        conf.setInt("input_id", 3);
        List<Pair<LongWritable, ProtobufWritable<MobData>>> res = this.driver
                .withInput(new LongWritable(57L),
                            asList(MobDataUtil.createAndWrap(
                                    NullWritable.get())))
                .run();
        assertEquals(1, res.size());
        ProtobufWritable<MobData> output = res.get(0).getSecond();
        output.setConverter(MobData.class);
        assertEquals(3, output.get().getInputId());
    }
}
