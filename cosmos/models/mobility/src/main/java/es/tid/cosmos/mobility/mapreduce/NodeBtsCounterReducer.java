package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;

/**
 *
 * @author dmicol
 */
public class NodeBtsCounterReducer extends Reducer<
        ProtobufWritable<NodeBts>, NullWritable, LongWritable,
        ProtobufWritable<BtsCounter>> {
    private LongWritable userId;
    
    @Override
    public void setup(Context context) {
        this.userId = new LongWritable();
    }
    
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
                          Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        int count = 0;
        for (NullWritable unused : values) {
            count++;
        }
        final NodeBts node = key.get();
        this.userId.set(node.getUserId());
        ProtobufWritable<BtsCounter> counter = BtsCounterUtil.createAndWrap(
                node.getPlaceId(), node.getWeekday(), node.getRange(), count);
        context.write(this.userId, counter);
    }
}
