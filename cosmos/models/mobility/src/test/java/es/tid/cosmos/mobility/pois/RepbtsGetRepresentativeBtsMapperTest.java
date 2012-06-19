package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.io.InputStream;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsMapperTest {
    private MapDriver<LongWritable, TypedProtobufWritable<BtsCounter>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<BtsCounter>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<BtsCounter>,
                ProtobufWritable<TwoInt>, TypedProtobufWritable<BtsCounter>>(
                        new RepbtsGetRepresentativeBtsMapper());
        InputStream configInput = MobilityConfiguration.class.getResource(
                "/mobility.properties").openStream();
        MobilityConfiguration conf = new MobilityConfiguration();
        conf.load(configInput);
        this.driver.setConfiguration(conf);
    }

    @Test
    public void testBelowThresholds() throws Exception {
        {
            final TypedProtobufWritable<BtsCounter> btsCounter =
                    new TypedProtobufWritable<BtsCounter>(
                            BtsCounterUtil.create(1L, 2, 3, 4));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
        {
            final TypedProtobufWritable<BtsCounter> btsCounter =
                    new TypedProtobufWritable<BtsCounter>(
                            BtsCounterUtil.create(1L, 2, 3, 20));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
        {
            final TypedProtobufWritable<BtsCounter> btsCounter =
                    new TypedProtobufWritable<BtsCounter>(
                            BtsCounterUtil.create(1L, 2, 20, 4));
            this.driver
                    .withInput(new LongWritable(5L), btsCounter)
                    .runTest();
        }
    }

    @Test
    public void testAboveThresholds() throws Exception {
        final TypedProtobufWritable<BtsCounter> btsCounter =
                new TypedProtobufWritable<BtsCounter>(
                        BtsCounterUtil.create(1L, 2, 20, 10));
        final TypedProtobufWritable<BtsCounter> outputBtsCounter =
                new TypedProtobufWritable<BtsCounter>(
                        BtsCounterUtil.create(1L, 0, 0, 10));
        this.driver
                .withInput(new LongWritable(5L), btsCounter)
                .withOutput(TwoIntUtil.createAndWrap(5L, 1L), outputBtsCounter)
                .runTest();
    }
}
