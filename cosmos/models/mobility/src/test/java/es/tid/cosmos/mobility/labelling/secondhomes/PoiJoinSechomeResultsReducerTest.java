package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiJoinSechomeResultsReducerTest {
    private ReduceDriver<ProtobufWritable<TwoInt>, MobilityWritable<Message>,
            ProtobufWritable<TwoInt>, MobilityWritable<Poi>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<TwoInt>,
                MobilityWritable<Message>, ProtobufWritable<TwoInt>,
                MobilityWritable<Poi>>(new PoiJoinSechomeResultsReducer());
    }

    @Test
    public void testOutputWithSecHomeCount() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                TwoIntUtil.create(52L, 37L));
        final MobilityWritable<Message> value3 = new MobilityWritable<Message>(
                Null.getDefaultInstance());
        final MobilityWritable<Poi> outValue = new MobilityWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 100, 100, 14,
                               15, 52, 37));
        this.driver
                .withInput(key, asList(value1, value2, value3))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testOutputWithoutSecHomeCount() {
        final ProtobufWritable<TwoInt> key = TwoIntUtil.createAndWrap(1L, 2L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                TwoIntUtil.create(52L, 37L));
        final MobilityWritable<Poi> outValue = new MobilityWritable<Poi>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 52, 37));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
