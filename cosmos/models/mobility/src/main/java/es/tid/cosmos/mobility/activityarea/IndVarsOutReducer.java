package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.RepeatedActivityAreasUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public class  IndVarsOutReducer extends Reducer<
        LongWritable,  ProtobufWritable<RepeatedActivityAreas>,
        NullWritable,  Text> {
    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<RepeatedActivityAreas>> values,
            Context context) throws IOException, InterruptedException {
        for (ProtobufWritable<RepeatedActivityAreas> value : values) {
            value.setConverter(RepeatedActivityAreas.class);
            final RepeatedActivityAreas activityAreas = value.get();
            context.write(NullWritable.get(),
                          new Text(key.toString() + "|" +
                          RepeatedActivityAreasUtil.toString(activityAreas)));
        }
    }
}
