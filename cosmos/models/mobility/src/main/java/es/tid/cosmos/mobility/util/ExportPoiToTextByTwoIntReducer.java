package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Null, Text>
 *
 * @author dmicol
 */
public class ExportPoiToTextByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Poi>, NullWritable, Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (MobilityWritable<Poi> value : values) {
            final Poi poi = value.get();
            context.write(NullWritable.get(),
                          new Text(TwoIntUtil.toString(twoInt)
                                   + PoiUtil.DELIMITER
                                   + PoiUtil.toString(poi)));
        }
    }
}
