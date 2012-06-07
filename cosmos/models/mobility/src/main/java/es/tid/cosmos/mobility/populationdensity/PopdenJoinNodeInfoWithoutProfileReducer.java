package es.tid.cosmos.mobility.populationdensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <Long, NodeBts>
 * Output: <BtsProfile, Int>
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoWithoutProfileReducer extends Reducer<
        LongWritable, ProtobufWritable<MobData>, ProtobufWritable<BtsProfile>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final NodeBts nodeBts = mobData.getNodeBts();
            context.write(BtsProfileUtil.createAndWrap(nodeBts.getBts(), 0,
                                  nodeBts.getWeekday(), nodeBts.getRange()),
                          MobDataUtil.createAndWrap(1));
        }
    }
}
