package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjGroupTypePoiClientReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, ProtobufWritable<PoiNew>,
            ProtobufWritable<TwoInt>, ProtobufWritable<TwoInt>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                ProtobufWritable<PoiNew>, ProtobufWritable<TwoInt>,
                ProtobufWritable<TwoInt>>(new AdjGroupTypePoiClientReducer());
    }
    
    @Test
    public void testSomeMethod() throws IOException {
        ProtobufWritable<PoiNew> pn1 = PoiNewUtil.createAndWrap(1, 2L, 3L, 4, 1);
        ProtobufWritable<PoiNew> pn2 = PoiNewUtil.createAndWrap(5, 6L, 7L, 8, 0);
        List<Pair<ProtobufWritable<TwoInt>, ProtobufWritable<TwoInt>>> results =
                this.driver
                        .withInput(TwoIntUtil.createAndWrap(1, 2),
                                   asList(pn1, pn2))
                .run();
        assertEquals(1, results.size());
        final ProtobufWritable<TwoInt> keyWrapper = results.get(0).getFirst();
        final ProtobufWritable<TwoInt> valueWrapper = results.get(0).getSecond();
        keyWrapper.setConverter(TwoInt.class);
        valueWrapper.setConverter(TwoInt.class);
        final TwoInt key = keyWrapper.get();
        final TwoInt value = valueWrapper.get();
        assertEquals(3L, key.getNum1());
        assertEquals(7L, key.getNum2());
        assertEquals(1L, value.getNum1());
        assertEquals(5L, value.getNum2());
    }
}
