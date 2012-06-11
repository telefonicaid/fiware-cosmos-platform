package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <NodeBts, Null>
 * Output: <Long, BtsCounter>
 *
 * @author dmicol
 */
public class NodeBtsCounterReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int count = 0;
        for (ProtobufWritable<MobData> value : values) {
            count++;
        }
        key.setConverter(NodeBts.class);
        final NodeBts node = key.get();
        BtsCounter counter = BtsCounterUtil.create(node.getBts(),
                node.getWeekday(), node.getRange(), count);
        context.write(new LongWritable(node.getUserId()),
                      MobDataUtil.createAndWrap(counter));
    }
}
