package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntNewReducerTest {
    private ReduceDriver<
            LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable,
                ProtobufWritable<MobData>, LongWritable,
                ProtobufWritable<MobData>>(
                        new AdjAddUniqueIdPoiToTwoIntReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                 0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<LongWritable, ProtobufWritable<MobData>>> result =
                this.driver
                        .withInput(new LongWritable(137L),
                                   asList(MobDataUtil.createAndWrap(poi)))
                        .run();
        assertEquals(1, result.size());
        final LongWritable key = result.get(0).getFirst();
        assertEquals(37L, key.get());
        ProtobufWritable<MobData> valueWrapper = result.get(0).getSecond();
        valueWrapper.setConverter(MobData.class);
        final TwoInt value = valueWrapper.get().getTwoInt();
        assertEquals(37L, value.getNum1());
        assertEquals(37L, value.getNum2());
    }
}
