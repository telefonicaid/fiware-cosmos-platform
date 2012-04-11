package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;

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
        List<BtsCounter> allBts = new ArrayList<BtsCounter>();
        int numberOfValues = 0;
        for (ProtobufWritable<BtsCounter> value : values) {
            numberOfValues++;
            final BtsCounter btsCounter = value.get();
            allBts.add(btsCounter);
        }
        ProtobufWritable<NodeMxCounter> nodeMxCounter =
                NodeMxCounterUtil.createAndWrap(allBts,
                                                numberOfValues,
                                                numberOfValues);
        context.write(key, nodeMxCounter);
    }
}
