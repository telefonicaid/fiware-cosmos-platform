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
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
                          Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        int count = 0;
        for (NullWritable unused : values) {
            count++;
        }
        NodeBts node = key.get();
        BtsCounter counter = BtsCounterUtil.create(node.getPlaceId(),
                                                   node.getWeekday(),
                                                   node.getRange(), count);
        ProtobufWritable<BtsCounter> wrapper = ProtobufWritable.newInstance(
                BtsCounter.class);
        wrapper.set(counter);
        context.write(new LongWritable(node.getUserId()), wrapper);
    }
}
