package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertPoiNewToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<PoiNew>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<PoiNew>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<PoiNew> value : values) {
            value.setConverter(PoiNew.class);
            final PoiNew poiNew = value.get();
            context.write(key, MobDataUtil.createAndWrap(poiNew));
        }
    }
}
