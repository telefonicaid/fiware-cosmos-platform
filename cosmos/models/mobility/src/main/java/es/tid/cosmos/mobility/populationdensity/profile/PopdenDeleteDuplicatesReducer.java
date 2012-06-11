package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDate;

/**
 * Input: <NodeBtsDate, Null>
 * Output: <Long, NodeBts>
 * 
 * @author ximo
 */
public class PopdenDeleteDuplicatesReducer extends Reducer<
        ProtobufWritable<NodeBtsDate>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBtsDate> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBtsDate.class);
        final NodeBtsDate nodeBtsDate = key.get();
        context.write(new LongWritable(nodeBtsDate.getUserId()),
                      MobDataUtil.createAndWrap(NodeBtsUtil.create(
                              nodeBtsDate.getUserId(),
                              nodeBtsDate.getBts(),
                              nodeBtsDate.getDate().getWeekday(),
                              nodeBtsDate.getHour())));
    }
}
