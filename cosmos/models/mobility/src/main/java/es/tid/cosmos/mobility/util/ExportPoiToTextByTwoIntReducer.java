package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class ExportPoiToTextByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, NullWritable, Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final Poi poi = value.get().getPoi();
            context.write(NullWritable.get(),
                          new Text(TwoIntUtil.toString(twoInt)
                                   + PoiUtil.DELIMITER
                                   + PoiUtil.toString(poi)));
        }
    }
}
