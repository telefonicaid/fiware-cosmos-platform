package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiPos;

/**
 *
 * @author dmicol
 */
public class ConvertPoiPosToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<PoiPos>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<PoiPos>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<PoiPos> value : values) {
            value.setConverter(PoiPos.class);
            final PoiPos poiPos = value.get();
            context.write(key, MobDataUtil.createAndWrap(poiPos));
        }
    }
}
