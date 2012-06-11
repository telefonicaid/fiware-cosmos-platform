package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <Long, NodeBts|Integer>
 * Output: <BtsProfile, Long>
 * 
 * @author ximo
 */
public class PopdenJoinNodeInfoProfileReducer extends Reducer<
        LongWritable, MobilityWritable<Message>,
        ProtobufWritable<BtsProfile>, MobilityWritable<Int64>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = MobilityWritable.divideIntoTypes(
                values, NodeBts.class, ClientProfile.class);
        List<NodeBts> nodebtsList = dividedLists.get(NodeBts.class);
        List<ClientProfile> profileList = dividedLists.get(ClientProfile.class);
        
        for (ClientProfile clientProfile : profileList) {
            final int profileId = clientProfile.getProfileId();
            for (NodeBts nodebts : nodebtsList) {
                context.write(BtsProfileUtil.createAndWrap(nodebts.getBts(),
                                      profileId, nodebts.getWeekday(),
                                      nodebts.getRange()),
                              MobilityWritable.create(1L));
            }
        }
    }
}
