package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiPosUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class GetPairsSechomePoisReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        Config.homeLabelgroupId = 3;
        Config.workLabelgroupId = 6;
        Config.minDistSecondHome = 49342.85D;
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>(
                        new GetPairsSechomePoisReducer());
    }

    @Test
    public void shouldProduceOutput() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(10, 20, 6, 40000.0D, 40000.0D, 0, 70, 80, 90,
                                  100, 110));
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(2, 20);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                1L);
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToDistance() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(1, 2, 3, 10000.0D, 10000.0D, 1, 7, 8, 9, 10,
                                  11));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(10, 20, 6, 40000.0D, 40000.0D, 0, 70, 80, 90,
                                  100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }
    
    @Test
    public void shouldNotProduceOutputDueToDistanceOneBeingZero() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(10, 20, 6, 4000.0D, 4000.0D, 0, 70, 80, 90,
                                  100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToLabelIdsInFirstValue() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(10, 20, 6, 40000.0D, 40000.0D, 0, 70, 80,
                                    90, 100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }

    @Test
    public void shouldNotProduceOutputDueToLabelIdsInSecondValue() {
        final LongWritable key = new LongWritable(57L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(1, 2, 3, 4, 5, 1, 7, 8, 9, 10, 11));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                PoiPosUtil.create(10, 20, 30, 40000.0D, 40000.0D, 0, 70, 80,
                                    90, 100, 110));
        this.driver
                .withInput(key, asList(value1, value2))
                .runTest();
    }
}
