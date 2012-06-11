package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class MatrixSpreadNodeMapperTest {
    private MapDriver<ProtobufWritable<TwoInt>, MobilityWritable<ItinTime>,
            LongWritable, MobilityWritable<ItinTime>> instance;
    
    @Before
    public void setUp() {
        this.instance = new MapDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<ItinTime>, LongWritable,
                MobilityWritable<ItinTime>>(new MatrixSpreadNodeMapper());
    }

    @Test
    public void testMap() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(57L,
                                                                      102L);
        final MobilityWritable<ItinTime> value = new MobilityWritable<ItinTime>(
                ItinTime.getDefaultInstance());
        final LongWritable outKey = new LongWritable(57L);
        this.instance
                .withInput(key, value)
                .withOutput(outKey, value)
                .runTest();
    }
}
