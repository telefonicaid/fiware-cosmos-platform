package es.tid.cosmos.mobility.propulationdensity.profile;

import java.io.IOException;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.when;
import org.powermock.api.mockito.PowerMockito;
import org.powermock.core.classloader.annotations.PrepareForTest;
import org.powermock.modules.junit4.PowerMockRunner;

import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.populationdensity.profile.PopdenSpreadNodebtsdayhourReducer;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 *
 * @author ximo
 */
@RunWith(PowerMockRunner.class)
@PrepareForTest(CellsCatalogue.class)
public class PopdenSpreadNodebtsdayhourReducerTest {
    private ReduceDriver<LongWritable, ProtobufWritable<MobData>,
            ProtobufWritable<NodeBtsDate>, ProtobufWritable<MobData>> instance;
    private List<Cell> cells;
    
    @Before
    public void setUp() throws IOException {
        this.cells = new LinkedList<Cell>();
        this.cells.add(CellUtil.create(10L, 11L, 12, 13, 14.0, 15.0));
        PowerMockito.mockStatic(CellsCatalogue.class);
        when(CellsCatalogue.load(any(Path.class), any(Configuration.class)))
                .thenReturn(this.cells);
        this.instance = new ReduceDriver<LongWritable, ProtobufWritable<MobData>,
                ProtobufWritable<NodeBtsDate>, ProtobufWritable<MobData>>(
                        new PopdenSpreadNodebtsdayhourReducer());
        this.instance.getConfiguration().set("cells", "/home/test");
    }

    @Test
    public void testReduce() {
        Date date = DateUtil.create(3, 4, 5, 6);
        ProtobufWritable<MobData> cdr = MobDataUtil.createAndWrap(
                CdrUtil.create(1L, 2L, date,
                               TimeUtil.create(7, 8, 9)));
        this.instance
                .withInput(new LongWritable(0L),
                           Arrays.asList(cdr))
                .withOutput(NodeBtsDateUtil.createAndWrap(1L, 11L, date, 7),
                            MobDataUtil.createAndWrap(NullWritable.get()))
                .runTest();
    }
}
