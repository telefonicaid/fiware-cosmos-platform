package es.tid.cosmos.mobility.util;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;
import java.io.InputStream;

/**
 *
 * @author dmicol
 */
public class ExportPoiToTextByTwoIntReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
            NullWritable, Text> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>, NullWritable, Text>(
                        new ExportPoiToTextByTwoIntReducer());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.driver.setConfiguration(Config.load(configInput,
                                                 this.driver.getConfiguration()));
    }
    
    @Test
    public void testSetInputId() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Poi> value = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final Text outValue = new Text("57|32|1|2|3|4|5|6|7.0|8|9|10|11.0|12|"
                + "13|14|15.0|16|17");
        this.driver
                .withInput(key, asList(value))
                .withOutput(NullWritable.get(), outValue)
                .runTest();
    }
}
