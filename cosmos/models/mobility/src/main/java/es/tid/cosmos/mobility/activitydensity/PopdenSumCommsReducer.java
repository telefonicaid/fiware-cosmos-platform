package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <BtsProfile, Int>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class PopdenSumCommsReducer extends Reducer<ProtobufWritable<BtsProfile>,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<BtsProfile> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int nComsTotal = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            nComsTotal += mobData.getInt();
        }
        key.setConverter(BtsProfile.class);
        final BtsProfile btsWdayhour = key.get();
        ProtobufWritable<TwoInt> btsProfile =
                TwoIntUtil.createAndWrap(btsWdayhour.getBts(),
                                         btsWdayhour.getProfile());
        ProtobufWritable<MobData> counter = MobDataUtil.createAndWrap(
                BtsCounterUtil.create(0L, btsWdayhour.getWday(),
                                      btsWdayhour.getHour(), nComsTotal));
        context.write(btsProfile, counter);
    }
}
