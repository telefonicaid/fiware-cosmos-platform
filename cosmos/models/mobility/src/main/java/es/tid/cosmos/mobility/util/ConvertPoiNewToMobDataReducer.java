package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;

/**
 *
 * @author dmicol
 */
public class ConvertPoiNewToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<PoiNew>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<PoiNew>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<PoiNew> value : values) {
            value.setConverter(PoiNew.class);
            final PoiNew poiNew = value.get();
            context.write(key, MobDataUtil.createAndWrap(poiNew));
        }
    }
}
