package es.tid.ps.mobility.jobs;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.MxProtocol.NodeBtsDay;
import es.tid.ps.mobility.data.NodeBtsDayUtil;

/**
 * Count NodeBtsDay
 *
 * @author sortega, dmicol
 */
public class MobmxRepbtsAggbybtsReducer extends
        Reducer<ProtobufWritable<NodeBtsDay>, IntWritable, LongWritable,
        ProtobufWritable<NodeBtsDay>> {
    private LongWritable outKey;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.outKey = new LongWritable();
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBtsDay> inKey,
            Iterable<IntWritable> callCounts, Context context)
            throws IOException, InterruptedException {
        inKey.setConverter(NodeBtsDay.class);
        this.outKey.set(inKey.get().getNode());

	int totalCallCount = 0;
        for (IntWritable callCount : callCounts) {
            totalCallCount += callCount.get();
        }

        ProtobufWritable<NodeBtsDay> nodeBtsDay = NodeBtsDayUtil.createAndWrap(
                inKey.get().getNode(),
                inKey.get().getBts(),
                inKey.get().getWorkday(),
                totalCallCount);
        context.write(this.outKey, nodeBtsDay);
    }
}
