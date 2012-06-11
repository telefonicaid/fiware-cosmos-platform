package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.io.InputStream;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsMapperTest {
    private MapDriver<LongWritable, MobilityWritable<BtsCounter>,
            ProtobufWritable<TwoInt>, MobilityWritable<BtsCounter>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new MapDriver<LongWritable, MobilityWritable<BtsCounter>,
                ProtobufWritable<TwoInt>, MobilityWritable<BtsCounter>>(
                        new RepbtsGetRepresentativeBtsMapper());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.driver.setConfiguration(Config.load(configInput,
                this.driver.getConfiguration()));
    }

    @Test
    public void testBelowThresholds() throws Exception {
        {
            final MobilityWritable<BtsCounter> btsCounter =
                    new MobilityWritable<BtsCounter>(
                            BtsCounterUtil.create(1L, 2, 3, 4));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
        {
            final MobilityWritable<BtsCounter> btsCounter =
                    new MobilityWritable<BtsCounter>(
                            BtsCounterUtil.create(1L, 2, 3, 20));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
        {
            final MobilityWritable<BtsCounter> btsCounter =
                    new MobilityWritable<BtsCounter>(
                            BtsCounterUtil.create(1L, 2, 20, 4));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
    }

    @Test
    public void testAboveThresholds() throws Exception {
        final MobilityWritable<BtsCounter> btsCounter =
                new MobilityWritable<BtsCounter>(
                        BtsCounterUtil.create(1L, 2, 20, 10));
        final MobilityWritable<BtsCounter> outputBtsCounter =
                new MobilityWritable<BtsCounter>(
                        BtsCounterUtil.create(1L, 0, 0, 10));
        this.driver
                .withInput(new LongWritable(5L), btsCounter)
                .withOutput(TwoIntUtil.createAndWrap(5L, 1L), outputBtsCounter)
                .runTest();
    }
}
