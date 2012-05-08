package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class AdjCountIndexesReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new AdjCountIndexesReducer());
    }
    
    @Test
    public void testReduce() {
        this.driver
                .withInput(new LongWritable(3L),
                           asList(MobDataUtil.createAndWrap(5L),
                                  MobDataUtil.createAndWrap(10L),
                                  MobDataUtil.createAndWrap(7L),
                                  MobDataUtil.createAndWrap(0L)))
                .withOutput(new LongWritable(22L),
                            MobDataUtil.createAndWrap(NullWritable.get()))
                .runTest();
    }
}
