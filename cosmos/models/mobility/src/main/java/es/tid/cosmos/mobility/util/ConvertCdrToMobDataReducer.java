package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ConvertCdrToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<Cell>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Cell>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Cell> value : values) {
            value.setConverter(Cell.class);
            final Cell cell = value.get();
            context.write(key, MobDataUtil.createAndWrap(cell));
        }
    }
}
