package es.tid.cosmos.mobility.propulationdensity.profile;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.DateUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenDeleteDuplicatesReducer;

/**
 *
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducerTest {
    private ReduceDriver<ProtobufWritable<MobData>, NullWritable,
            LongWritable, ProtobufWritable<MobData>> instance;
    
    @Before
    public void setUp() {
        this.instance = new ReduceDriver<ProtobufWritable<MobData>,
                NullWritable, LongWritable, ProtobufWritable<MobData>>(
                        new PopdenDeleteDuplicatesReducer());
    }

    @Test
    public void testReduce() {
        NodeBtsDate key = NodeBtsDateUtil.create(
                1L, 2L, DateUtil.create(3, 4, 5, 6), 7);
        this.instance
                .withInput(MobDataUtil.createAndWrap(key),
                           asList(NullWritable.get()))
                .withOutput(new LongWritable(1L),
                            MobDataUtil.createAndWrap(
                                    NodeBtsUtil.create(1L, 2L, 6, 7)))
                .runTest();
    }
}
