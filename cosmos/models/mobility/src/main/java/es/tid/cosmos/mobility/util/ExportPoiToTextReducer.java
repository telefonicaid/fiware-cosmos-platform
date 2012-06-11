package es.tid.cosmos.mobility.util;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 * Input: <Long, Poi>
 * Output: <Null, Text>
 *
 * @author dmicol
 */
public class ExportPoiToTextReducer extends Reducer<LongWritable,
        MobilityWritable<Poi>, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<Poi> value : values) {
            final Poi poi = value.get();
            context.write(NullWritable.get(),
                          new Text(key + PoiUtil.DELIMITER
                                   + PoiUtil.toString(poi)));
        }
    }
}
