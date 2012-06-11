package es.tid.cosmos.mobility.labelling.join;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 *
 * @author dmicol
 */
public class ClusterAggNodeClusterByNodlblReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>, LongWritable,
            MobilityWritable<Poi>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                LongWritable, MobilityWritable<Poi>>(
                        new ClusterAggNodeClusterByNodlblReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(57L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                ClusterUtil.create(1, 2, 0, 3, 4,
                                   ClusterVector.getDefaultInstance()));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 0, 6, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        final LongWritable outKey = new LongWritable(1L);
        final MobilityWritable<Poi> outValue = new MobilityWritable<Poi>(
                PoiUtil.create(1, 2, 3, 1, 2, 0, 4, 7, 8, 0, 9, 10, 11, 0, 13,
                               14, 15));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
