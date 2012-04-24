package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;

/**
 * Count NodeBtsDay
 *
 * @author dmicol, sortega
 */
public class RepbtsAggbybtsReducer extends Reducer<ProtobufWritable<NodeBtsDay>,
        IntWritable, LongWritable, ProtobufWritable<NodeBtsDay>> {
    @Override
    public void reduce(ProtobufWritable<NodeBtsDay> key,
            Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        int totalCallCount = 0;
        for (IntWritable callCount : values) {
            totalCallCount += callCount.get();
        }

                key.setConverter(NodeBtsDay.class);
        final NodeBtsDay byDay = key.get();
        ProtobufWritable<NodeBtsDay> nodeBtsDay = NodeBtsDayUtil.createAndWrap(
                byDay.getUserId(),
                byDay.getPlaceId(),
                byDay.getWorkday(),
                totalCallCount);
        context.write(new LongWritable(byDay.getUserId()), nodeBtsDay);
    }
}
