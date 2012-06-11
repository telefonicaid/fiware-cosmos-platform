package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <TwoInt, NodeMxCounter>
 * Output: <BtsProfile, Int>
 * 
 * @author dmicol
 */
public class PopdenSpreadArrayReducer extends Reducer<LongWritable,
        MobilityWritable<NodeMxCounter>, ProtobufWritable<BtsProfile>,
        MobilityWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<NodeMxCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<NodeMxCounter> value : values) {
            final NodeMxCounter counter = value.get();
            for (BtsCounter btsCounter : counter.getBtsList()) {
                final ProtobufWritable<BtsProfile> btsProfile = 
                        BtsProfileUtil.createAndWrap(btsCounter.getBts(), 0,
                                                     btsCounter.getWeekday(),
                                                     btsCounter.getRange());
                context.write(btsProfile,
                              MobilityWritable.create(btsCounter.getCount()));
            }
        }
    }
}
