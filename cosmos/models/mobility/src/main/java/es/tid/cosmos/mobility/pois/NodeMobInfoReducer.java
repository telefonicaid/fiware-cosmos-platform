package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;
import es.tid.cosmos.mobility.data.NodeMxCounterUtil;

/**
 * Input: <Long, BtsCounter>
 * Output: <Long, NodeMxCounter>
 * 
 * @author dmicol
 */
public class NodeMobInfoReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<BtsCounter> allBts = new ArrayList<BtsCounter>();
        int numberOfValues = 0;
        for (ProtobufWritable<MobData> value : values) {
            numberOfValues++;
            value.setConverter(MobData.class);
            final BtsCounter btsCounter = value.get().getBtsCounter();
            allBts.add(btsCounter);
        }
        NodeMxCounter nodeMxCounter = NodeMxCounterUtil.create(allBts,
                                                               numberOfValues,
                                                               numberOfValues);
        context.write(key, MobDataUtil.createAndWrap(nodeMxCounter));
    }
}
