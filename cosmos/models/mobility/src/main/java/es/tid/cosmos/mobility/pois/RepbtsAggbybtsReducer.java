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
    private LongWritable userId;

    @Override
    public void setup(Context context) throws IOException,
                                              InterruptedException {
        this.userId = new LongWritable();
    }

    @Override
    public void reduce(ProtobufWritable<NodeBtsDay> key,
            Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBtsDay.class);
        final NodeBtsDay byDay = key.get();
        this.userId.set(byDay.getUserId());

	int totalCallCount = 0;
        for (IntWritable callCount : values) {
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
