package es.tid.cosmos.mobility.propulationdensity.profile;

import java.util.Arrays;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenSpreadNodebtsdayhourReducer;

/**
 *
 * @author ximo
 */
public class PopdenSpreadNodebtsdayhourReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBtsDate>, NullWritable> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
        ProtobufWritable<NodeBtsDate>, NullWritable>(
            new PopdenSpreadNodebtsdayhourReducer());
    }

    @Test
    public void testReduce() throws Exception {
        Date date = DateUtil.create(3, 4, 5, 6);
        ProtobufWritable<MobData> cdr = MobDataUtil.createAndWrap(
                CdrUtil.create(1L, 2L, date,
                               TimeUtil.create(7, 8, 9)));
        ProtobufWritable<MobData> cell = MobDataUtil.createAndWrap(
                CellUtil.create(10L, 11L, 12, 13, 14.0, 15.0));
        this.driver
                .withInput(new LongWritable(0L),
                           Arrays.asList(cdr, cell))
                .withOutput(NodeBtsDateUtil.createAndWrap(1L, 11L, date, 7),
                            NullWritable.get())
                .runTest();
    }
}
