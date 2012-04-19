package es.tid.cosmos.mobility.labeljoining;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class ClusterSpreadNodelblPoilblReducer extends Reducer<
        LongWritable, ProtobufWritable<Poi>, ProtobufWritable<TwoInt>,
        NullWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Poi> value : values) {
            value.setConverter(Poi.class);
            final Poi poi = value.get();
            if (poi.getConfidentnode() == 1 && poi.getConfidentbts() == 1) {
                context.write(TwoIntUtil.createAndWrap(poi.getLabelnode(),
                                                       poi.getLabelnodebts()),
                              NullWritable.get());
            }
        }
    }
}
