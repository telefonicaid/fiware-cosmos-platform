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

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToPoiNewReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Poi>,
            ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Poi>,
                ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>>(
                        new AdjAddUniqueIdPoiToPoiNewReducer());
    }
    
    @Test
    public void testReduce() throws IOException {
        Poi poi = PoiUtil.create(1, 2L, 3, 4, 5, 1, 4.3D, 6, 7,
                                 0, 9.1D, 10, 11, 1, 8.45D, 1, 0);
        List<Pair<ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>>> result =
                this.driver
                        .withInput(new LongWritable(137L),
                                   asList(new MobilityWritable<Poi>(poi)))
                        .run();
        assertEquals(1, result.size());
        ProtobufWritable<TwoInt> keyWrapper = result.get(0).getFirst();
        keyWrapper.setConverter(TwoInt.class);
        TwoInt key = keyWrapper.get();
        assertEquals(2, key.getNum1());
        assertEquals(11, key.getNum2());
        MobilityWritable<PoiNew> valueWrapper = result.get(0).getSecond();
        PoiNew value = valueWrapper.get();
        assertEquals(37, value.getId());
    }
}
