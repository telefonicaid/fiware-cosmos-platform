package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiPosReducerTest {
    private ReduceDriver<LongWritable, MobilityWritable<Message>, LongWritable,
            MobilityWritable<PoiPos>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, MobilityWritable<Message>,
                LongWritable, MobilityWritable<PoiPos>>(
                        new PoiJoinPoisBtscoordToPoiPosReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(1L);
        final MobilityWritable<Message> value1 = new MobilityWritable<Message>(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final MobilityWritable<Message> value2 = new MobilityWritable<Message>(
                CellUtil.create(1, 2, 3, 4, 5, 6));
        final LongWritable outKey = new LongWritable(2L);
        final MobilityWritable<PoiPos> outValue = new MobilityWritable<PoiPos>(
                PoiPosUtil.create(2, 3, 13, 5, 6, 16, 17, -1, -1, -1, -1));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
