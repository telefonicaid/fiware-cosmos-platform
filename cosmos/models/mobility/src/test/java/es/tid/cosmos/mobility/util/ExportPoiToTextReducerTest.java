package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ExportPoiToTextReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, NullWritable,
            Text> driver;
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                NullWritable, Text>(new ExportPoiToTextReducer());
    }
    
    @Test
    public void testSetInputId() throws IOException {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final Text outValue = new Text("57|1|2|3|4|5|6|7.0|8|9|10|11.0|12|13|"
                + "14|15.0|16|17");
        this.driver
                .withInput(key, asList(value))
                .withOutput(NullWritable.get(), outValue)
                .runTest();
    }
}
