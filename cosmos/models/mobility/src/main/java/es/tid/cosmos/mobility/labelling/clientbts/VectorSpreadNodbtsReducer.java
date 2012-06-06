package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <Long, NodeMxCounter>
 * Output: <NodeBts, Int>
 * 
 * @author dmicol
 */
public class VectorSpreadNodbtsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<NodeBts>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final NodeMxCounter nodeMxCounter = value.get().getNodeMxCounter();
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
                              MobDataUtil.createAndWrap(btsCounter.getCount()));
            }
        }
    }
}
