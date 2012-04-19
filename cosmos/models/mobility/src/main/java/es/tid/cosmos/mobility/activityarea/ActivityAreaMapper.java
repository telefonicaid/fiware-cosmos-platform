package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.util.Pair;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.TelMonth;

/**
 *
 * @author losa
 */
public class ActivityAreaMapper extends Mapper<
        ProtobufWritable<TelMonth>, ProtobufWritable<Cell>,
        LongWritable,
        Pair<ProtobufWritable<TelMonth>, ProtobufWritable<Cell>>> {
    @Override
    protected void map(ProtobufWritable<TelMonth> key,
            ProtobufWritable<Cell> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TelMonth.class);
        final TelMonth readKey = key.get();
        LongWritable phoneKey = new LongWritable(readKey.getPhone());
        context.write(phoneKey, new Pair(key, value));
    }
}
