package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;
import java.io.InputStream;
import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsUtil;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;

/**
 *
 * @author dmicol
 */
public class FilterBtsVectorReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>, LongWritable,
            MobilityWritable<Cluster>> driver;
    @Before
    public void setUp() throws IOException {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                LongWritable, MobilityWritable<Cluster>>(
                        new FilterBtsVectorReducer());
        InputStream configInput = Config.class.getResource(
                "/mobility.properties").openStream();
        this.driver.setConfiguration(Config.load(configInput,
                this.driver.getConfiguration()));
    }

    @Test
    public void testNonConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                BtsUtil.create(1, 50000, 2, 3, 6, asList(5L, 6L, 7L)));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final MobilityWritable<Cluster> outValue = new MobilityWritable<Cluster>(
                ClusterUtil.create(1, 2, 0, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }

    @Test
    public void testConfidentOutput() {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                BtsUtil.create(1, 80000, 2, 3, 4, asList(5L, 6L, 7L)));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        final MobilityWritable<Cluster> outValue = new MobilityWritable<Cluster>(
                ClusterUtil.create(1, 2, 1, 4, 5,
                                   ClusterVector.getDefaultInstance()));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(key, outValue)
                .runTest();
    }
}
