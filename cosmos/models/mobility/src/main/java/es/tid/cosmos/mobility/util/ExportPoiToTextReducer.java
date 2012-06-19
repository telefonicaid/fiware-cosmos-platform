package es.tid.cosmos.mobility.util;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 * Input: <Long, Poi>
 * Output: <Null, Text>
 *
 * @author dmicol
 */
public class ExportPoiToTextReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Poi>, NullWritable, Text> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.separator = conf.getDataSeparator();
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<Poi> value : values) {
            final Poi poi = value.get();
            context.write(NullWritable.get(),
                          new Text(key + this.separator
                                   + PoiUtil.toString(poi, this.separator)));
        }
    }
}
