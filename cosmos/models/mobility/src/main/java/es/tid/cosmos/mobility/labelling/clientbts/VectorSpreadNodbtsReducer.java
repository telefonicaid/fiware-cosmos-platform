package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <Long, NodeMxCounter>
 * Output: <NodeBts, Int>
 * 
 * @author dmicol
 */
public class VectorSpreadNodbtsReducer extends Reducer<LongWritable,
        MobilityWritable<NodeMxCounter>, ProtobufWritable<NodeBts>,
        MobilityWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<NodeMxCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<NodeMxCounter> value : values) {
            final NodeMxCounter nodeMxCounter = value.get();
            for (BtsCounter btsCounter : nodeMxCounter.getBtsList()) {
                int group;
                switch (btsCounter.getWeekday()) {
                    case 0:
                        group = 3;
                        break;
                    case 5:
                        group = 1;
                        break;
                    case 6:
                        group = 2;
                        break;
                    default:
                        group = 0;
                }
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        key.get(), btsCounter.getBts(),
                        group, btsCounter.getRange());
                context.write(nodeBts,
                              MobilityWritable.create(btsCounter.getCount()));
            }
        }
    }
}
