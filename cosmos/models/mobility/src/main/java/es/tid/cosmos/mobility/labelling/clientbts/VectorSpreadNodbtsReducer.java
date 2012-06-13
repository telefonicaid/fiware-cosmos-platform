package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <Long, NodeMxCounter>
 * Output: <NodeBts, Int>
 * 
 * @author dmicol
 */
public class VectorSpreadNodbtsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBts>,
        TypedProtobufWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<NodeMxCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<NodeMxCounter> value : values) {
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
                              TypedProtobufWritable.create(btsCounter.getCount()));
            }
        }
    }
}
