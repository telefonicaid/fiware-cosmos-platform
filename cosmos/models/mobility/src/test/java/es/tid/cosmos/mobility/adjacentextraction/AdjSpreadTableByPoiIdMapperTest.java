package es.tid.cosmos.mobility.adjacentextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<TwoInt>, LongWritable,
            LongWritable> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, ProtobufWritable<TwoInt>,
                LongWritable, LongWritable>(new AdjSpreadTableByPoiIdMapper());
    }
    
    @Test
    public void testSomeMethod() {
        this.driver
                .withInput(new LongWritable(57L),
                           TwoIntUtil.createAndWrap(3L, 6L))
                .withOutput(new LongWritable(3L), new LongWritable(6L))
                .runTest();
    }
}
