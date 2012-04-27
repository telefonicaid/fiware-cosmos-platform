package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;

/**
 *
 * @author dmicol
 */
public class ConvertNodeBtsDayToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<NodeBtsDay>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<NodeBtsDay>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<NodeBtsDay> value : values) {
            value.setConverter(NodeBtsDay.class);
            final NodeBtsDay nodeBtsDay = value.get();
            context.write(key, MobDataUtil.createAndWrap(nodeBtsDay));
        }
    }
}
