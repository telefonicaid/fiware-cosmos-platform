package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.when;
import org.powermock.api.mockito.PowerMockito;
import org.powermock.core.classloader.annotations.PrepareForTest;
import org.powermock.modules.junit4.PowerMockRunner;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.*;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 *
 * @author ximo
 */
@RunWith(PowerMockRunner.class)
@PrepareForTest(CellsCatalogue.class)
public class PopdenSpreadNodebtsdayhourReducerTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<Cdr>,
            ProtobufWritable<NodeBtsDate>, TypedProtobufWritable<Null>> instance;
    private List<Cell> cells;
    
    @Before
    public void setUp() throws IOException {
        this.cells = new LinkedList<Cell>();
        this.cells.add(CellUtil.create(10L, 11L, 12, 13, 14.0, 15.0));
        PowerMockito.mockStatic(CellsCatalogue.class);
        when(CellsCatalogue.load(any(Path.class), any(MobilityConfiguration.class)))
                .thenReturn(this.cells);
        this.instance = new ReduceDriver<LongWritable, TypedProtobufWritable<Cdr>,
                ProtobufWritable<NodeBtsDate>, TypedProtobufWritable<Null>>(
                        new PopdenSpreadNodebtsdayhourReducer());
        InputStream configInput = MobilityConfiguration.class.getResource(
                "/mobility.properties").openStream();
        MobilityConfiguration conf = new MobilityConfiguration();
        conf.load(configInput);
        conf.set("cells", "/home/test");
        this.instance.setConfiguration(conf);
    }

    @Test
    public void testReduce() {
        when(CellsCatalogue.filter(this.cells, 10L)).thenReturn(this.cells);
        Date date = DateUtil.create(3, 4, 5, 6);
        TypedProtobufWritable<Cdr> cdr = new TypedProtobufWritable<Cdr>(
                CdrUtil.create(1L, 2L, date, TimeUtil.create(7, 8, 9)));
        this.instance
                .withInput(new LongWritable(10L), Arrays.asList(cdr))
                .withOutput(NodeBtsDateUtil.createAndWrap(1L, 11L, date, 7),
                            new TypedProtobufWritable<Null>(Null.getDefaultInstance()))
                .runTest();
    }
}
