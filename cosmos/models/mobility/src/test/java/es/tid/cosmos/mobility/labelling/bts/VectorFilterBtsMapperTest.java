package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorFilterBtsMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<BtsCounter>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<BtsCounter>, ProtobufWritable<MobData>>(
                        new VectorFilterBtsMapper());
    }
    
    @Test
    public void testMap() throws IOException {
        final LongWritable key = new LongWritable(3L);
        final TwoInt value = TwoIntUtil.create(4L, 7L);
        final BtsCounter outKey = BtsCounterUtil.create(3L, 4, 7, 0);
        this.driver
                .withInput(key, MobDataUtil.createAndWrap(value))
                .withOutput(BtsCounterUtil.wrap(outKey),
                            MobDataUtil.createAndWrap(NullWritable.get()))
                .runTest();
    }
}
