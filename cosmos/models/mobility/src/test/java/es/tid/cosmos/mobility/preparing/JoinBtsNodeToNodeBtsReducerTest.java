package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import static java.util.Arrays.asList;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.mockito.Mockito.any;
import static org.mockito.Mockito.when;
import org.powermock.api.mockito.PowerMockito;
import org.powermock.core.classloader.annotations.PrepareForTest;
import org.powermock.modules.junit4.PowerMockRunner;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 *
 * @author dmicol
 */
@RunWith(PowerMockRunner.class)
@PrepareForTest(CellsCatalogue.class)
public class JoinBtsNodeToNodeBtsReducerTest {
    private List<Cell> cells;
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() throws IOException {
        this.cells = new LinkedList<Cell>();
        cells.add(CellUtil.create(1L, 2L, 3, 4, 5L, 6L));
        cells.add(CellUtil.create(10L, 20L, 30, 40, 50L, 60L));
        cells.add(CellUtil.create(33L, 200L, 300, 400, 500L, 600L));
        cells.add(CellUtil.create(10L, 2000L, 3000, 4000, 5000L, 6000L));
        PowerMockito.mockStatic(CellsCatalogue.class);
        when(CellsCatalogue.load(any(Path.class), any(Configuration.class)))
                .thenReturn(cells);
        //this.driver = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
        //        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>>(
         //               new JoinBtsNodeToNodeBtsReducer());
        this.driver.getConfiguration().set("cells", "/home/test");
    }

    @Test
    public void testEmptyOutput() throws IOException {
        List<Cell> filteredCells = new LinkedList<Cell>();
        when(CellsCatalogue.filter(this.cells, 57L)).thenReturn(filteredCells);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                CdrUtil.create(1L, 2L, Date.getDefaultInstance(),
                               Time.getDefaultInstance()));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                CdrUtil.create(3L, 4L, Date.getDefaultInstance(),
                               Time.getDefaultInstance()));
        List<Pair<ProtobufWritable<NodeBts>, ProtobufWritable<MobData>>> res =
                this.driver
                        .withInput(new LongWritable(57L),
                                   asList(value1, value2))
                        .run();
        assertNotNull(res);
        assertEquals(0, res.size());
    }
    
    @Test
    public void testNonEmptyOutput() throws IOException {
        List<Cell> filteredCells = new LinkedList<Cell>();
        filteredCells.add(this.cells.get(0));
        filteredCells.add(this.cells.get(2));
        when(CellsCatalogue.filter(this.cells, 10L)).thenReturn(filteredCells);
        final ProtobufWritable<MobData> value1 = MobDataUtil.createAndWrap(
                CdrUtil.create(1L, 2L, Date.getDefaultInstance(),
                               Time.getDefaultInstance()));
        final ProtobufWritable<MobData> value2 = MobDataUtil.createAndWrap(
                CdrUtil.create(3L, 4L, Date.getDefaultInstance(),
                               Time.getDefaultInstance()));
        List<Pair<ProtobufWritable<NodeBts>, ProtobufWritable<MobData>>> res =
                this.driver
                        .withInput(new LongWritable(10L),
                                   asList(value1, value2))
                        .run();
        assertNotNull(res);
        assertEquals(4, res.size());
    }
}
