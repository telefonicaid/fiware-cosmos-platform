package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjJoinNewPoiIdReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>,
            ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>>(
                        new AdjJoinNewPoiIdReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(103L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                Int64.newBuilder().setNum(57L).build());
        final MobilityWritable<Message> value2 =
                new MobilityWritable<Message>(PoiNewUtil.create(1, 2, 3, 4, 0));
        final MobilityWritable<Message> value3 = new MobilityWritable<Message>(
                Int64.newBuilder().setNum(32L).build());
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 3);
        final MobilityWritable<PoiNew> outValue1 = new MobilityWritable<PoiNew>(
                PoiNewUtil.create(57, 2, 3, 4, 0));
        final MobilityWritable<PoiNew> outValue2 = new MobilityWritable<PoiNew>(
                PoiNewUtil.create(32, 2, 3, 4, 0));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(outKey, outValue1)
                .withOutput(outKey, outValue2)
                .runTest();
    }
}
