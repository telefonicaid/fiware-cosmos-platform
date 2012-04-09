package es.tid.cosmos.mobility.mapreduce;

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
 * @author sortega, dmicol
 */
public class RepbtsAggbybtsReducer extends
        Reducer<ProtobufWritable<NodeBtsDay>, IntWritable, LongWritable,
        ProtobufWritable<NodeBtsDay>> {
    private LongWritable userId;

    @Override
    public void setup(Context context) throws IOException,
                                              InterruptedException {
        this.userId = new LongWritable();
    }

    @Override
    public void reduce(ProtobufWritable<NodeBtsDay> inKey,
            Iterable<IntWritable> callCounts, Context context)
            throws IOException, InterruptedException {
        inKey.setConverter(NodeBtsDay.class);
        final NodeBtsDay byDay = inKey.get();
        this.userId.set(byDay.getUserId());

	int totalCallCount = 0;
        for (IntWritable callCount : callCounts) {
            totalCallCount += callCount.get();
        }

        ProtobufWritable<NodeBtsDay> nodeBtsDay = NodeBtsDayUtil.createAndWrap(
                byDay.getUserId(),
                byDay.getPlaceId(),
                byDay.getWorkday(),
                totalCallCount);
        context.write(this.userId, nodeBtsDay);
    }
}
