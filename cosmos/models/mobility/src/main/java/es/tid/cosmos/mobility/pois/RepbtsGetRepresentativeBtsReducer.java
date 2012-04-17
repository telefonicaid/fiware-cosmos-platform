package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<BtsCounter>, ProtobufWritable<TwoInt>,
        ProtobufWritable<BtsCounter>> {
    private static final int MOB_CONF_MIN_PERC_REP_BTS = 5;
    private static final int MOB_CONF_MIN_NUMBER_CALLS_BTS = 14;

    @Override
    public void reduce(LongWritable key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<BtsCounter> value : values) {
            value.setConverter(BtsCounter.class);
            final BtsCounter counter = value.get();
            if (counter.getCount() >= MOB_CONF_MIN_PERC_REP_BTS
                    && counter.getRange() >= MOB_CONF_MIN_NUMBER_CALLS_BTS) {
                ProtobufWritable<TwoInt> nodeBts = TwoIntUtil.createAndWrap(
                        key.get(), counter.getPlaceId());
                ProtobufWritable<BtsCounter> btsCounter =
                        BtsCounterUtil.createAndWrap(counter.getPlaceId(), 0,
                                                     0, counter.getCount());
                context.write(nodeBts, btsCounter);
            }
        }
    }
}
