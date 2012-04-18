package es.tid.cosmos.mobility.clientbtslabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;
import es.tid.cosmos.mobility.data.NodeBtsUtil;

/**
 *
 * @author dmicol
 */
public class VectorSpreadNodbtsReducer extends Reducer<LongWritable,
        ProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBts>,
        IntWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<NodeMxCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<NodeMxCounter> value : values) {
            value.setConverter(NodeMxCounter.class);
            final NodeMxCounter nodeMxCounter = value.get();
            for (BtsCounter btsCounter : nodeMxCounter.getBtsList()) {
                int group;
                switch (btsCounter.getWeekday()) {
                    case 0:
                        group = 3;
                        break;
                    case 1:
                        group = 1;
                        break;
                    case 6:
                        group = 2;
                        break;
                    default:
                        group = 0;
                }
                ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                        key.get(), (int)btsCounter.getPlaceId(),
                        group, btsCounter.getRange());
                context.write(nodeBts, new IntWritable(btsCounter.getCount()));
            }
        }
    }
}
