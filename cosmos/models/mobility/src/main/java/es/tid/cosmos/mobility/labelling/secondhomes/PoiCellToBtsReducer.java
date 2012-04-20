package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class PoiCellToBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<Cell>, LongWritable, ProtobufWritable<Cell>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Cell>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Cell> value : values) {
            value.setConverter(Cell.class);
            final Cell cell = value.get();
            context.write(new LongWritable(cell.getPlaceId()), value);
        }
    }
}
