package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <Long, Cdr|NodeBsDay>
 * Output: <Long, Int>
 * 
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    public void reduce(LongWritable key,
                       Iterable<ProtobufWritable<MobData>> values,
                       Context context)
            throws IOException, InterruptedException {
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
                    break;
                default:
                    throw new IllegalArgumentException();
            }
        }
        int totalComms = numCommsInfo + numCommsNoInfoOrNoBts;
        if (totalComms >= Config.minTotalCalls &&
                totalComms <= Config.maxTotalCalls) {
            context.write(key, MobDataUtil.createAndWrap(numCommsInfo));
        }
    }
}
