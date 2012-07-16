package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <Long, BtsCounter>
 * Output: <Long, NodeMxCounter>
 *
 * @author dmicol
 */
class NodeMobInfoReducer extends Reducer<LongWritable,
        TypedProtobufWritable<BtsCounter>, LongWritable,
        TypedProtobufWritable<NodeMxCounter>> {

    @Override
    public void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        List<BtsCounter> allBts = new ArrayList<BtsCounter>();
        for (TypedProtobufWritable<BtsCounter> value : values) {
            final BtsCounter btsCounter = value.get();
            allBts.add(btsCounter);
        }
        NodeMxCounter nodeMxCounter = NodeMxCounterUtil.create(allBts);
        context.write(key,
                      new TypedProtobufWritable<NodeMxCounter>(nodeMxCounter));
    }
}
