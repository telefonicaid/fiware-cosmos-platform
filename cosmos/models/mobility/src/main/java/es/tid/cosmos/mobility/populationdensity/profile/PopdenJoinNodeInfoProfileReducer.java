package es.tid.cosmos.mobility.populationdensity.profile;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoProfileReducer extends Reducer<
        LongWritable, ProtobufWritable<MobData>,
        ProtobufWritable<MobData>, LongWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<NodeBts> nodebtsList = new LinkedList<NodeBts>();
        List<Integer> profileIdList = new LinkedList<Integer>();        
        for (final ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch(mobData.getType()) {
                case NODE_BTS:
                    nodebtsList.add(mobData.getNodeBts());
                    break;
                case INT:
                    profileIdList.add(mobData.getInt());
                    break;
                default:
                    throw new IllegalStateException("Invalid data type: "
                            + mobData.getType().toString());
            }
        }
        
        for (int profileId : profileIdList) {
            for(NodeBts nodebts : nodebtsList) {
                context.write(MobDataUtil.createAndWrap(
                        BtsProfileUtil.create(
                                nodebts.getBts(), profileId,
                                nodebts.getWeekday(), nodebts.getRange())),
                        new LongWritable(1L));
            }
        }
    }
}
