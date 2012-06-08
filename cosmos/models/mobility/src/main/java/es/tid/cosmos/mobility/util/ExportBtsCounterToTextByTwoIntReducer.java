package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ExportBtsCounterToTextByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<BtsCounter>, NullWritable,
        Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (MobilityWritable<BtsCounter> value : values) {
            final BtsCounter btsCounter = value.get();
            context.write(NullWritable.get(),
                          new Text(TwoIntUtil.toString(twoInt)
                                   + BtsCounterUtil.DELIMITER
                                   + BtsCounterUtil.toString(btsCounter)));
        }
    }
}
