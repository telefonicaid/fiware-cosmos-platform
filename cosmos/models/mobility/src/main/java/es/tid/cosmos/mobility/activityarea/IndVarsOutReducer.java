package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.ActivityAreaKeyUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;

/**
 *
 * @author losa
 */
public class  IndVarsOutReducer extends Reducer<
        ProtobufWritable<ActivityAreaKey>,  ProtobufWritable<ActivityArea>,
        NullWritable,  Text> {
    @Override
    public void reduce(ProtobufWritable<ActivityAreaKey> key,
            Iterable<ProtobufWritable<ActivityArea>> values, Context context) 
            throws IOException, InterruptedException {
        key.setConverter(ActivityAreaKey.class);
        final ActivityAreaKey actAreaKey = key.get();
        for (ProtobufWritable<ActivityArea> value : values) {
            value.setConverter(ActivityArea.class);
            final ActivityArea activityArea = value.get();
            context.write(NullWritable.get(),
                          new Text(ActivityAreaKeyUtil.toString(actAreaKey) 
                          + "|" + ActivityAreaUtil.toString(activityArea)));
        }
    }
}
