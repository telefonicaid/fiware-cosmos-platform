package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public class  FusionTotalVarsReducer extends Reducer<
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>,
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>> {
    @Override
    protected void reduce(ProtobufWritable<ActivityAreaKey> key,
            Iterable<ProtobufWritable<ActivityArea>> values, Context context)
            throws IOException, InterruptedException {
    }
}
