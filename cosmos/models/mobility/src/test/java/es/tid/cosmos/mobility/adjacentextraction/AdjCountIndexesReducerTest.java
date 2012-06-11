package es.tid.cosmos.mobility.adjacentextraction;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;

/**
 *
 * @author dmicol
 */
public class AdjCountIndexesReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Int64>, LongWritable,
            MobilityWritable<Null>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Int64>,
                LongWritable, MobilityWritable<Null>>(
                        new AdjCountIndexesReducer());
    }
    
    @Test
    public void testReduce() {
        this.driver
                .withInput(new LongWritable(3L),
                           asList(MobilityWritable.create(5L),
                                  MobilityWritable.create(10L),
                                  MobilityWritable.create(7L),
                                  MobilityWritable.create(0L)))
                .withOutput(new LongWritable(22L),
                            new MobilityWritable<Null>(Null.getDefaultInstance()))
                .runTest();
    }
}
