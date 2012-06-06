package es.tid.cosmos.mobility.propulationdensity.profile;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenDeleteDuplicatesReducer;

/**
 *
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducerTest {
    private ReduceDriver<ProtobufWritable<NodeBtsDate>, NullWritable,
            LongWritable, ProtobufWritable<NodeBts>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBtsDate>,
                NullWritable, LongWritable, ProtobufWritable<NodeBts>>(
                        new PopdenDeleteDuplicatesReducer());
    }

    @Test
    public void testReduce() throws Exception {
        ProtobufWritable<NodeBtsDate> key = NodeBtsDateUtil.createAndWrap(
                1L, 2L, DateUtil.create(3, 4, 5, 6), 7);
        this.driver
                .withInput(key, asList(NullWritable.get()))
                .withOutput(new LongWritable(1L),
                            NodeBtsUtil.createAndWrap(1L, 2L, 6, 7))
                .runTest();
    }
}
