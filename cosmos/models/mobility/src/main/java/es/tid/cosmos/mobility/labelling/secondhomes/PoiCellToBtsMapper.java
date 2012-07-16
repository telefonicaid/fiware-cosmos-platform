package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 * Input: <Long, Cell>
 * Output: <Long, Cell>
 *
 * @author dmicol
 */
class PoiCellToBtsMapper extends Mapper<LongWritable,
        TypedProtobufWritable<Cell>, LongWritable, TypedProtobufWritable<Cell>> {
    @Override
    protected void map(LongWritable key, TypedProtobufWritable<Cell> value,
            Context context) throws IOException, InterruptedException {
        final Cell cell = value.get();
        context.write(new LongWritable(cell.getBts()), value);
    }
}
