package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <Long, BtsCounter>
 * Output: <Long, NodeMxCounter>
 * 
 * @author dmicol
 */
public class NodeMobInfoReducer extends Reducer<LongWritable,
        MobilityWritable<BtsCounter>, LongWritable, MobilityWritable<NodeMxCounter>> {
    @Override
    public void reduce(LongWritable key,
            Iterable<MobilityWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        List<BtsCounter> allBts = new ArrayList<BtsCounter>();
        int numberOfValues = 0;
        for (MobilityWritable<BtsCounter> value : values) {
            numberOfValues++;
            final BtsCounter btsCounter = value.get();
            allBts.add(btsCounter);
        }
        NodeMxCounter nodeMxCounter = NodeMxCounterUtil.create(allBts,
                                                               numberOfValues,
                                                               numberOfValues);
        context.write(key, new MobilityWritable<NodeMxCounter>(nodeMxCounter));
    }
}
