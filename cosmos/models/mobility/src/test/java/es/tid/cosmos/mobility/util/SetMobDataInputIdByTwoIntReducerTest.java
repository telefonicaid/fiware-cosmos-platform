package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class SetMobDataInputIdByTwoIntReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<Message>,
            ProtobufWritable<TwoInt>, MobilityWritable<InputIdRecord>> driver;
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<Message>, ProtobufWritable<TwoInt>,
                MobilityWritable<InputIdRecord>>(
                        new SetMobDataInputIdByTwoIntReducer());
    }
    
    @Test(expected=IllegalArgumentException.class)
    public void testNoInputId() {
        this.driver.runTest();
    }
    
    @Test
    public void testSetInputId() throws IOException {
        Configuration conf = this.driver.getConfiguration();
        conf.setInt("input_id", 3);
        List<Pair<ProtobufWritable<TwoInt>, MobilityWritable<InputIdRecord>>> res =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(1L, 2L),
                                   asList(new MobilityWritable<Message>(
                                           Null.getDefaultInstance())))
                        .run();
        assertEquals(1, res.size());
        MobilityWritable<InputIdRecord> output = res.get(0).getSecond();
        assertEquals(3, output.get().getInputId());
    }
}
