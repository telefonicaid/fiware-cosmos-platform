package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, Cell>
 * Output: <Long, Cell>
 * 
 * @author dmicol
 */
public class PoiCellToBtsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void map(LongWritable key, ProtobufWritable<MobData> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final Cell cell = value.get().getCell();
        context.write(new LongWritable(cell.getBts()), value);
    }
}
