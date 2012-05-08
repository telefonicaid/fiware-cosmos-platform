package es.tid.cosmos.mobility.pois;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<TwoInt>, ProtobufWritable<MobData>>(
                        new RepbtsGetRepresentativeBtsMapper());
    }

    @Test
    public void testBelowThresholds() throws Exception {
        {
            final ProtobufWritable<MobData> btsCounter =
                    MobDataUtil.createAndWrap(
                            BtsCounterUtil.create(1L, 2, 3, 4));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
        {
            final ProtobufWritable<MobData> btsCounter =
                    MobDataUtil.createAndWrap(
                            BtsCounterUtil.create(1L, 2, 3, 20));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
        {
            final ProtobufWritable<MobData> btsCounter =
                    MobDataUtil.createAndWrap(
                            BtsCounterUtil.create(1L, 2, 20, 4));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
    }

    @Test
    public void testAboveThresholds() throws Exception {
        final ProtobufWritable<MobData> btsCounter = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(1L, 2, 20, 10));
        final ProtobufWritable<MobData> outputBtsCounter =
                MobDataUtil.createAndWrap(BtsCounterUtil.create(1L, 0, 0, 10));
        this.driver
                .withInput(new LongWritable(5L), btsCounter)
                .withOutput(TwoIntUtil.createAndWrap(5L, 1L), outputBtsCounter)
                .runTest();
    }
}
