package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <Long, Int|NodeBtsDay>
 * Output: <Long, BtsCounter>
 * 
 * @author dmicol
 */
public class RepbtsJoinDistCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Integer> ncommsList = new LinkedList<Integer>();
        List<NodeBtsDay> nodeBtsDayList = new LinkedList<NodeBtsDay>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case INT:
                    ncommsList.add(mobData.getInt());
                    break;
                case NODE_BTS_DAY:
                    nodeBtsDayList.add(mobData.getNodeBtsDay());
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }
        for (Integer ncomms : ncommsList) {
            for (NodeBtsDay nodeBtsDay : nodeBtsDayList) {
                BtsCounter counter = BtsCounterUtil.create(
                                nodeBtsDay.getBts(),
                                0,
                                nodeBtsDay.getCount(),
                                nodeBtsDay.getCount() * 100 / ncomms);
                context.write(key, MobDataUtil.createAndWrap(counter));
            }
        }
    }
}
