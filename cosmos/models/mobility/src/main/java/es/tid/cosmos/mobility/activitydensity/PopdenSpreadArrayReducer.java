package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <TwoInt, NodeMxCounter>
 * Output: <BtsProfile, Int>
 * 
 * @author dmicol
 */
class PopdenSpreadArrayReducer extends Reducer<LongWritable,
        TypedProtobufWritable<NodeMxCounter>, ProtobufWritable<BtsProfile>,
        TypedProtobufWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<NodeMxCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<NodeMxCounter> value : values) {
            final NodeMxCounter counter = value.get();
            for (BtsCounter btsCounter : counter.getBtsList()) {
                final ProtobufWritable<BtsProfile> btsProfile = 
                        BtsProfileUtil.createAndWrap(btsCounter.getBts(), 0,
                                                     btsCounter.getWeekday(),
                                                     btsCounter.getRange());
                context.write(btsProfile,
                              TypedProtobufWritable.create(btsCounter.getCount()));
            }
        }
    }
}
