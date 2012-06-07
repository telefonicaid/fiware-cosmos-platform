package es.tid.cosmos.mobility.activitydensity.profile;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <TwoInt, NodeMxCounter|Int>
 * Output: <BtsProfile, Int>
 * 
 * @author dmicol
 */
public class PopdenJoinArrayProfileReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<BtsProfile>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<NodeMxCounter> counterList = new LinkedList<NodeMxCounter>();
        List<Integer> profileList = new LinkedList<Integer>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case NODE_MX_COUNTER:
                    counterList.add(mobData.getNodeMxCounter());
                    break;
                case INT:
                    profileList.add(mobData.getInt());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        for (int profile : profileList) {
            for (NodeMxCounter counter : counterList) {
                for (BtsCounter btsCounter : counter.getBtsList()) {
                    final ProtobufWritable<BtsProfile> btsProfile =
                            BtsProfileUtil.createAndWrap(
                                    btsCounter.getBts(), profile,
                                    btsCounter.getWeekday(),
                                    btsCounter.getRange());
                    context.write(btsProfile,
                            MobDataUtil.createAndWrap(btsCounter.getCount()));
                }
            }
        }
    }
}
