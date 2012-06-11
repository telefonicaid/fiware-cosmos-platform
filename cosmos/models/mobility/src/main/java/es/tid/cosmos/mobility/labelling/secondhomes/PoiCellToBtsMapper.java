package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 * Input: <Long, Cell>
 * Output: <Long, Cell>
 * 
 * @author dmicol
 */
public class PoiCellToBtsMapper extends Mapper<LongWritable,
        MobilityWritable<Cell>, LongWritable, MobilityWritable<Cell>> {
    @Override
    protected void map(LongWritable key, MobilityWritable<Cell> value,
            Context context) throws IOException, InterruptedException {
        final Cell cell = value.get();
        context.write(new LongWritable(cell.getBts()), value);
    }
}
