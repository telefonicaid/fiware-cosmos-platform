package es.tid.cosmos.mobility.labelling.secondhomes;

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiPosUtil;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiPosReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new PoiJoinPoisBtscoordToPoiPosReducer());
    }

    @Test
    public void testReduce() {
        final LongWritable key = new LongWritable(1L);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                PoiUtil.create(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
                               15, 16, 17));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                CellUtil.create(1, 2, 3, 4, 5, 6));
        final LongWritable outKey = new LongWritable(2L);
        final ProtobufWritable<MobData> outValue = MobDataUtil.createAndWrap(
                PoiPosUtil.create(2, 3, 13, 5, 6, 16, 17, -1, -1, -1, -1));
        this.driver
                .withInput(key, asList(value1, value2))
                .withOutput(outKey, outValue)
                .runTest();
    }
}
