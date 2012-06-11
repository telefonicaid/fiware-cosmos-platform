package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsDateUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <NodeBtsDate, Null>
 *
 * @author ximo
 */
public class PopdenSpreadNodebtsdayhourReducer extends Reducer<LongWritable,
        MobilityWritable<Cdr>, ProtobufWritable<NodeBtsDate>,
        MobilityWritable<Null>> {
    private static List<Cell> cells = null;
    
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (cells == null) {
            final Configuration conf = context.getConfiguration();
            cells = CellsCatalogue.load(new Path(conf.get("cells")), conf);
        }
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        final MobilityWritable<Null> nullValue = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        List<Cell> filteredCells = CellsCatalogue.filter(cells, key.get());
        for (final MobilityWritable<Cdr> value : values) {
            final Cdr cdr = value.get();
            for (Cell cell : filteredCells) {
                context.write(NodeBtsDateUtil.createAndWrap(cdr.getUserId(),
                                      cell.getBts(), cdr.getDate(),
                                      cdr.getTime().getHour()),
                              nullValue);
            }
        }
    }
}
