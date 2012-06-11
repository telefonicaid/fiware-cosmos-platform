package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;

/**
 * Input: <NodeBts, Null>
 * Output: <Long, BtsCounter>
 *
 * @author dmicol
 */
public class NodeBtsCounterReducer extends Reducer<
        ProtobufWritable<NodeBts>, MobilityWritable<Null>, LongWritable,
        MobilityWritable<BtsCounter>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<MobilityWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        int count = 0;
        for (MobilityWritable<Null> value : values) {
            count++;
        }
        key.setConverter(NodeBts.class);
        final NodeBts node = key.get();
        BtsCounter counter = BtsCounterUtil.create(node.getBts(),
                node.getWeekday(), node.getRange(), count);
        context.write(new LongWritable(node.getUserId()),
                      new MobilityWritable<BtsCounter>(counter));
    }
}
