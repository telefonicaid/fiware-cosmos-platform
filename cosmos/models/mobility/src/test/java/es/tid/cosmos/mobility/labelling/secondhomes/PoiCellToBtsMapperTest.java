package es.tid.cosmos.mobility.labelling.secondhomes;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class PoiCellToBtsMapperTest {
    private MapDriver<LongWritable, MobilityWritable<Cell>, LongWritable,
            MobilityWritable<Cell>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, MobilityWritable<Cell>,
                LongWritable, MobilityWritable<Cell>>(new PoiCellToBtsMapper());
    }

    @Test
    public void testReduce() {
        MobilityWritable<Cell> value = new MobilityWritable<Cell>(
                CellUtil.create(12321L, 432L, 40, 50, 3.21D, 54.5D));
        this.driver
                .withInput(new LongWritable(57L), value)
                .withOutput(new LongWritable(432L), value)
                .runTest();
    }
}
