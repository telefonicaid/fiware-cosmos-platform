package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;

/**
 *
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, IntWritable> {
    private static final int MIN_TOTAL_CALLS = 200;
    private static final int MAX_TOTAL_CALLS = 5000;

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
        if (totalComms >= MIN_TOTAL_CALLS && totalComms <= MAX_TOTAL_CALLS) {
            context.write(key, new IntWritable(numCommsInfo));
        }
    }
}
