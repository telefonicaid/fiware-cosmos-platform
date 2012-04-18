package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.RepeatedActivityAreasUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public class FusionTotalVarsReducer extends Reducer<
        LongWritable,
        ProtobufWritable<ActivityArea>,
        LongWritable,
        ProtobufWritable<RepeatedActivityAreas>> {
    private List<ActivityArea> allAreas;

    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<ActivityArea>> values, Context context)
            throws IOException, InterruptedException {
        this.allAreas = new ArrayList<ActivityArea>();
        for (ProtobufWritable<ActivityArea> value: values) {
            value.setConverter(ActivityArea.class);
            allAreas.add(value.get());
        }
        ProtobufWritable<RepeatedActivityAreas> wrappedAreas =
            RepeatedActivityAreasUtil.createAndWrap(allAreas);
        context.write(key, wrappedAreas);
    }
}
