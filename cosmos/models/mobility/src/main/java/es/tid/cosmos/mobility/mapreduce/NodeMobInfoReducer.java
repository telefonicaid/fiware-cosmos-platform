package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class NodeMobInfoReducer extends Reducer<LongWritable,
        ProtobufWritable<BtsCounter>, LongWritable,
        ProtobufWritable<NodeMxCounter>> {
    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        NodeMxCounter.Builder nodeMxCounterBuilder = NodeMxCounter.newBuilder();
        int numberOfValues = 0;
        for (ProtobufWritable<BtsCounter> value : values) {
            numberOfValues++;
            BtsCounter btsCounter = value.get();
            nodeMxCounterBuilder.addBts(btsCounter);
        }
        nodeMxCounterBuilder.setBtsLength(numberOfValues);
        nodeMxCounterBuilder.setBtsMaxLength(numberOfValues);
        NodeMxCounter nodeMxCounter = nodeMxCounterBuilder.build();

        ProtobufWritable<NodeMxCounter> nodeMxCounterWrapper =
                ProtobufWritable.newInstance(NodeMxCounter.class);
        nodeMxCounterWrapper.set(nodeMxCounter);
        context.write(key, nodeMxCounterWrapper);
    }
}
