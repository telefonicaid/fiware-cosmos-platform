package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, BtsCounter>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    private static final int MOB_CONF_MIN_PERC_REP_BTS = 5;
    private static final int MOB_CONF_MIN_NUMBER_CALLS_BTS = 14;

    @Override
    public void map(LongWritable key, ProtobufWritable<MobData> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final BtsCounter counter = value.get().getBtsCounter();
        if (counter.getCount() >= MOB_CONF_MIN_PERC_REP_BTS
                && counter.getRange() >= MOB_CONF_MIN_NUMBER_CALLS_BTS) {
            ProtobufWritable<TwoInt> nodeBts = TwoIntUtil.createAndWrap(
                    key.get(), counter.getPlaceId());
            BtsCounter btsCounter = BtsCounterUtil.create(counter.getPlaceId(),
                    0, 0, counter.getCount());
            context.write(nodeBts, MobDataUtil.createAndWrap(btsCounter));
        }
    }
}
