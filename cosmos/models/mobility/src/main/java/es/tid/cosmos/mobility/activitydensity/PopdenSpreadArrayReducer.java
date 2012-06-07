package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

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
 * Input: <TwoInt, NodeMxCounter>
 * Output: <BtsProfile, Int>
 * 
 * @author dmicol
 */
public class PopdenSpreadArrayReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<BtsProfile>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final NodeMxCounter counter = mobData.getNodeMxCounter();
            for (BtsCounter btsCounter : counter.getBtsList()) {
                final ProtobufWritable<BtsProfile> btsProfile = 
                        BtsProfileUtil.createAndWrap(btsCounter.getBts(), 0,
                                                     btsCounter.getWeekday(),
                                                     btsCounter.getRange());
                context.write(btsProfile,
                              MobDataUtil.createAndWrap(btsCounter.getCount()));
            }
        }
    }
}
