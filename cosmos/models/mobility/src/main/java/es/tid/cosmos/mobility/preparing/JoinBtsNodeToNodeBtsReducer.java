package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <NodeBts, Null>
 * 
 * @author dmicol
 */
public class JoinBtsNodeToNodeBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
        ProtobufWritable<MobData>> {
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
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = CellsCatalogue.filter(cells, key.get());
        if (filteredCells.isEmpty()) {
            return;
        }
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final Cdr cdr = value.get().getCdr();
            for (Cell cell : filteredCells) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        cdr.getUserId(), cell.getBts(),
                        cdr.getDate().getWeekday(), cdr.getTime().getHour());
                context.write(nodeBts,
                              MobDataUtil.createAndWrap(NullWritable.get()));
            }
        }
    }
}
