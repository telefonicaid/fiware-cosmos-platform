package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.util.Logger;

/**
 *
 * @author dmicol
 */
public class RepbtsJoinDistCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<BtsCounter>> {
    private static final int MIN_TOTAL_CALLS = 200;
    private static final int MAX_TOTAL_CALLS = 5000;

    @Override
    public void reduce(LongWritable key,
                       Iterable<ProtobufWritable<MobData>> values,
                       Context context)
            throws IOException, InterruptedException {
        List<NodeBtsDay> nodeBtsDayList = new LinkedList<NodeBtsDay>();
        int numCommsInfo = 0;
        int numCommsNoInfoOrNoBts = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case CDR:
                    numCommsNoInfoOrNoBts++;
                    break;
                case NODE_BTS_DAY:
                    final NodeBtsDay nodeBtsDay = mobData.getNodeBtsDay();
                    numCommsInfo += nodeBtsDay.getCount();
                    nodeBtsDayList.add(nodeBtsDay);
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }
        Logger.get().info("NumCommsInfo: " + numCommsInfo);
        Logger.get().info("NumCommsNoInfoOrNoBts: " + numCommsNoInfoOrNoBts);
        int totalComms = numCommsInfo + numCommsNoInfoOrNoBts;
        if (totalComms >= MIN_TOTAL_CALLS && totalComms <= MAX_TOTAL_CALLS) {
            for (NodeBtsDay nodeBtsDay : nodeBtsDayList) {
                ProtobufWritable<BtsCounter> counter =
                        BtsCounterUtil.createAndWrap(
                                nodeBtsDay.getPlaceId(),
                                0,
                                nodeBtsDay.getCount(),
                                nodeBtsDay.getCount() * 100 / totalComms);
                context.write(key, counter);
            }                                         
        }
    }
}
