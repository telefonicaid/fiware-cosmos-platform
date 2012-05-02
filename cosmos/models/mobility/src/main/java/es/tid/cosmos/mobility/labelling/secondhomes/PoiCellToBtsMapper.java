package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public class PoiCellToBtsMapper extends Mapper<LongWritable,
        ProtobufWritable<Cell>, LongWritable, ProtobufWritable<Cell>> {
    @Override
    protected void map(LongWritable key, ProtobufWritable<Cell> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(Cell.class);
        final Cell cell = value.get();
        context.write(new LongWritable(cell.getPlaceId()), value);
    }
}
