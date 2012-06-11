package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntNewReducerTest {
    private ReduceDriver<
            LongWritable, MobilityWritable<Poi>, LongWritable,
            MobilityWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable,
                MobilityWritable<Poi>, LongWritable,
                MobilityWritable<TwoInt>>(
                        new AdjAddUniqueIdPoiToTwoIntReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                 0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<LongWritable, MobilityWritable<TwoInt>>> result =
                this.driver
                        .withInput(new LongWritable(137L),
                                   asList(new MobilityWritable<Poi>(poi)))
                        .run();
        assertEquals(1, result.size());
        final LongWritable key = result.get(0).getFirst();
        assertEquals(37L, key.get());
        MobilityWritable<TwoInt> valueWrapper = result.get(0).getSecond();
        final TwoInt value = valueWrapper.get();
        assertEquals(37L, value.getNum1());
        assertEquals(37L, value.getNum2());
    }
}
