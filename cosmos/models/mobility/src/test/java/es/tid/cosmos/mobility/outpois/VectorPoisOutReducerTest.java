package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static java.util.Arrays.asList;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorPoisOutReducerTest extends ConfiguredTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
            NullWritable, Text> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                TypedProtobufWritable<Poi>, NullWritable, Text>(
                        new VectorPoisOutReducer());
        this.driver.setConfiguration(this.getConf());
    }

    @Test
    public void testReduce() throws IOException {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L, 32L);
        final TypedProtobufWritable<Poi> value1 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final TypedProtobufWritable<Poi> value2 = new TypedProtobufWritable<Poi>(
                PoiUtil.create(10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110,
                               120, 130, 140, 150, 160, 170));
        List<Pair<NullWritable, Text>> results = this.driver
                .withInput(key, asList(value1, value2))
                .run();
        assertNotNull(results);
        assertEquals(2, results.size());
        Text firstResult = results.get(0).getSecond();
        assertEquals(14, firstResult.toString().split("\\|").length);
        Text secondResult = results.get(1).getSecond();
        assertEquals(14, secondResult.toString().split("\\|").length);
    }
}
