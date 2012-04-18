package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public class FusionTotalVarsReducer extends Reducer<
        LongWritable,
        ProtobufWritable<ActivityArea>,
        LongWritable,
        List<ProtobufWritable<ActivityArea>>> {
    private List<ProtobufWritable<ActivityArea>> allAreas;

    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<ActivityArea>> values, Context context)
            throws IOException, InterruptedException {
        this.allAreas = new ArrayList<ProtobufWritable<ActivityArea>>();
        for (ProtobufWritable<ActivityArea> value : values) {
            allAreas.add(value);
        }
        context.write(key, allAreas);
    }
}
