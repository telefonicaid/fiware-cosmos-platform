package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsReducer extends Reducer<IntWritable,
        ProtobufWritable<BtsCounter>, NullWritable, Text> {
    private static final int MOB_CONF_MIN_PERC_REP_BTS = 5;
    private static final int MOB_CONF_MIN_NUMBER_CALLS_BTS = 14;

    @Override
    public void reduce(IntWritable key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<BtsCounter> value : values) {
            final BtsCounter counter = value.get();
            if (counter.getCount() >= MOB_CONF_MIN_PERC_REP_BTS
                    && counter.getRange() >= MOB_CONF_MIN_NUMBER_CALLS_BTS) {
                TwoInt nodeBts = TwoIntUtil.create(key.get(),
                                                   counter.getPlaceId());
                BtsCounter btsCounter = BtsCounterUtil.create(
                        counter.getPlaceId(), 0, 0, counter.getCount());
                context.write(NullWritable.get(), new Text(
                        TwoIntUtil.toString(nodeBts) + "|" +
                        BtsCounterUtil.toString(btsCounter)));
            }
        }
    }
}
