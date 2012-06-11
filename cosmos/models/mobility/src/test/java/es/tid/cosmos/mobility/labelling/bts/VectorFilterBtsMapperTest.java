package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorFilterBtsMapperTest {
    private MapDriver<LongWritable, MobilityWritable<TwoInt>,
            ProtobufWritable<BtsCounter>, MobilityWritable<Null>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, MobilityWritable<TwoInt>,
                ProtobufWritable<BtsCounter>, MobilityWritable<Null>>(
                        new VectorFilterBtsMapper());
    }
    
    @Test
    public void testMap() throws IOException {
        final LongWritable key = new LongWritable(3L);
        final TwoInt value = TwoIntUtil.create(4L, 7L);
        final BtsCounter outKey = BtsCounterUtil.create(3L, 4, 7, 0);
        this.driver
                .withInput(key, new MobilityWritable<TwoInt>(value))
                .withOutput(BtsCounterUtil.wrap(outKey),
                            new MobilityWritable<Null>(Null.getDefaultInstance()))
                .runTest();
    }
}
