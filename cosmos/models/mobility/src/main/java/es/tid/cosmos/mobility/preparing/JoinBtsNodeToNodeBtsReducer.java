package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 *
 * @author dmicol
 */
public class JoinBtsNodeToNodeBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<Cdr>, ProtobufWritable<NodeBts>, NullWritable> {
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
            Iterable<ProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = new LinkedList<Cell>();
        for (Cell cell : cells) {
            if (cell.getCellId() == key.get()) {
                filteredCells.add(cell);
            }
        }
        if (filteredCells.isEmpty()) {
            return;
        }
        for (ProtobufWritable<Cdr> value : values) {
            value.setConverter(Cdr.class);
            final Cdr cdr = value.get();
            for (Cell cell : filteredCells) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        cdr.getUserId(), (int)cell.getPlaceId(),
                        cdr.getDate().getWeekday(), cdr.getTime().getHour());
                context.write(nodeBts, NullWritable.get());
            }
        }
    }
}
