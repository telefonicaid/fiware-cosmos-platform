package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <NodeBts, Null>
 * 
 * @author dmicol
 */
class JoinBtsNodeToNodeBtsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cdr>, ProtobufWritable<NodeBts>,
        TypedProtobufWritable<Null>> {
    private static List<Cell> cells = null;
    
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (cells == null) {
            final MobilityConfiguration conf =
                    (MobilityConfiguration) context.getConfiguration();
            cells = CellsCatalogue.load(new Path(conf.get("cells")), conf);
        }
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        List<Cell> filteredCells = CellsCatalogue.filter(cells, key.get());
        if (filteredCells.isEmpty()) {
            return;
        }
        for (TypedProtobufWritable<Cdr> value : values) {
            final Cdr cdr = value.get();
            for (Cell cell : filteredCells) {
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        cdr.getUserId(), cell.getBts(),
                        cdr.getDate().getWeekday(), cdr.getTime().getHour());
                context.write(nodeBts,
                              new TypedProtobufWritable<Null>(
                                      Null.getDefaultInstance()));
            }
        }
    }
}
