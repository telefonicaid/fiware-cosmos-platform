package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
class JoinBtsNodeToBtsDayRangeReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cdr>, LongWritable, TypedProtobufWritable<TwoInt>> {
    private static List<Cell> cells = null;
    
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (cells == null) {
            final MobilityConfiguration conf = new MobilityConfiguration(context.
                    getConfiguration());
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
                int group;
                switch (cdr.getDate().getWeekday()) {
                    case 0:
                        group = 3;
                        break;
                    case 5:
                        group = 1;
                        break;
                    case 6:
                        group = 2;
                        break;
                    default:
                        group = 0;
                }
                context.write(new LongWritable(cell.getBts()),
                        new TypedProtobufWritable<TwoInt>(
                                TwoIntUtil.create(group,
                                                  cdr.getTime().getHour())));
            }
        }
    }
}
