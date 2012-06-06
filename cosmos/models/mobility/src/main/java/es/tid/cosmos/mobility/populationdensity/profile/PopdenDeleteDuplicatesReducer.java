package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;

/**
 *
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducer extends Reducer<
        ProtobufWritable<NodeBtsDate>, NullWritable, LongWritable,
        ProtobufWritable<NodeBts>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBtsDate> key,
            Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBtsDate.class);
        final NodeBtsDate nodeBtsDate = key.get();
        context.write(
                new LongWritable(nodeBtsDate.getUserId()),
                NodeBtsUtil.createAndWrap(
                        nodeBtsDate.getUserId(),
                        nodeBtsDate.getBts(),
                        nodeBtsDate.getDate().getWeekday(),
                        nodeBtsDate.getHour()));
    }
}
