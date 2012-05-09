package es.tid.cosmos.mobility.labelling.secondhomes;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class PoiCellToBtsMapperTest {
    private MapDriver<LongWritable, ProtobufWritable<MobData>, LongWritable,
            ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, ProtobufWritable<MobData>,
                LongWritable, ProtobufWritable<MobData>>(
                        new PoiCellToBtsMapper());
    }

    @Test
    public void testReduce() {
        ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                CellUtil.create(12321L, 432L, 40, 50, 3.21D, 54.5D));
        this.driver
                .withInput(new LongWritable(57L), value)
                .withOutput(new LongWritable(432L), value)
                .runTest();
    }
}
