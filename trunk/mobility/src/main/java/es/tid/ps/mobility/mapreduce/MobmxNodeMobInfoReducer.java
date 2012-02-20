package es.tid.ps.mobility.mapreduce;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.MxProtocol.BtsCounter;
import es.tid.ps.mobility.data.MxProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class MobmxNodeMobInfoReducer extends Reducer<IntWritable,
        ProtobufWritable<BtsCounter>, IntWritable,
        ProtobufWritable<NodeMxCounter>> {
    @Override
    protected void reduce(IntWritable key,
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
