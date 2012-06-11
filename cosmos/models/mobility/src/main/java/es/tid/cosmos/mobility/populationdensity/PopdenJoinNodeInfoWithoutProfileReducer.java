package es.tid.cosmos.mobility.populationdensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <Long, NodeBts>
 * Output: <BtsProfile, Int>
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoWithoutProfileReducer extends Reducer<
        LongWritable, MobilityWritable<NodeBts>, ProtobufWritable<BtsProfile>,
        MobilityWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<NodeBts>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<NodeBts> value : values) {
            final NodeBts nodeBts = value.get();
            context.write(BtsProfileUtil.createAndWrap(nodeBts.getBts(), 0,
                                  nodeBts.getWeekday(), nodeBts.getRange()),
                          MobilityWritable.create(1));
        }
    }
}
