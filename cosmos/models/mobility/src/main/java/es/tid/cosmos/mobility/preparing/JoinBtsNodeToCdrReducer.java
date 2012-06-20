package es.tid.cosmos.mobility.preparing;

import java.io.IOException;
import java.util.List;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.util.CellsCatalogue;

/**
 * Input: <Long, Cdr>
 * Output: <Long, Cdr>
 * 
 * @author dmicol
 */
class JoinBtsNodeToCdrReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Cdr>, LongWritable, TypedProtobufWritable<Cdr>> {
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
            for (TypedProtobufWritable<Cdr> value : values) {
                final Cdr cdr = value.get();
                context.write(new LongWritable(cdr.getUserId()),
                              new TypedProtobufWritable<Cdr>(cdr));
            }
        }
    }
}
