package es.tid.cosmos.mobility.activitydensity.profile;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <TwoInt, NodeMxCounter|Int>
 * Output: <BtsProfile, Int>
 * 
 * @author dmicol
 */
public class PopdenJoinArrayProfileReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, ProtobufWritable<BtsProfile>,
        TypedProtobufWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        List<NodeMxCounter> counterList = new LinkedList<NodeMxCounter>();
        List<Integer> profileIdList = new LinkedList<Integer>();
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof NodeMxCounter) {
                counterList.add((NodeMxCounter)message);
            } else if (message instanceof ClientProfile) {
                profileIdList.add(((ClientProfile)message).getProfileId());
            } else {
                throw new IllegalStateException("Unexpected input: "
                        + message.toString());
            }
        }
        for (int profileId : profileIdList) {
            for (NodeMxCounter counter : counterList) {
                for (BtsCounter btsCounter : counter.getBtsList()) {
                    final ProtobufWritable<BtsProfile> btsProfile =
                            BtsProfileUtil.createAndWrap(
                                    btsCounter.getBts(), profileId,
                                    btsCounter.getWeekday(),
                                    btsCounter.getRange());
                    context.write(btsProfile,
                            TypedProtobufWritable.create(btsCounter.getCount()));
                }
            }
        }
    }
}
