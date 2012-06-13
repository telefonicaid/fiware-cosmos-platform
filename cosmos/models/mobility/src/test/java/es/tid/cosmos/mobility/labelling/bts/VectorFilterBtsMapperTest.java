package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorFilterBtsMapperTest {
    private MapDriver<LongWritable, TypedProtobufWritable<TwoInt>,
            ProtobufWritable<BtsCounter>, TypedProtobufWritable<Null>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<TwoInt>,
                ProtobufWritable<BtsCounter>, TypedProtobufWritable<Null>>(
                        new VectorFilterBtsMapper());
    }
    
    @Test
    public void testMap() throws IOException {
        final LongWritable key = new LongWritable(3L);
        final TwoInt value = TwoIntUtil.create(4L, 7L);
        final BtsCounter outKey = BtsCounterUtil.create(3L, 4, 7, 0);
        this.driver
                .withInput(key, new TypedProtobufWritable<TwoInt>(value))
                .withOutput(BtsCounterUtil.wrap(outKey),
                            new TypedProtobufWritable<Null>(Null.getDefaultInstance()))
                .runTest();
    }
}
