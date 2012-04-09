package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class NodeMobInfoReducer extends Reducer<IntWritable,
        ProtobufWritable<BtsCounter>, IntWritable,
        ProtobufWritable<NodeMxCounter>> {
    @Override
    public void reduce(IntWritable key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        NodeMxCounter.Builder nodeMxCounterBuilder = NodeMxCounter.newBuilder();
        NodeMxCounter nodeMxCounter = nodeMxCounterBuilder.build();

        int valueCount = 0;
        for (ProtobufWritable<BtsCounter> value : values) {
            valueCount++;
            BtsCounter btsCounter = value.get();
            nodeMxCounterBuilder.addBts(btsCounter);
            nodeMxCounterBuilder.setBtsLength(valueCount);
            nodeMxCounterBuilder.setBtsMaxLength(valueCount);
        }

        ProtobufWritable<NodeMxCounter> nodeMxCounterWrapper =
                ProtobufWritable.newInstance(NodeMxCounter.class);
        nodeMxCounterWrapper.set(nodeMxCounter);
        context.write(key, nodeMxCounterWrapper);
    }
}
