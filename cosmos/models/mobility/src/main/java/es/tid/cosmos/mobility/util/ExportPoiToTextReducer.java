package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 *
 * @author dmicol
 */
public class ExportPoiToTextReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final Poi poi = value.get().getPoi();
            context.write(NullWritable.get(),
                          new Text(key + PoiUtil.DELIMITER
                                   + PoiUtil.toString(poi)));
        }
    }
}
