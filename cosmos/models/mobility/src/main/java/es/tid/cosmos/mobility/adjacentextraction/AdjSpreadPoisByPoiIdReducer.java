package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;

/**
 *
 * @author dmicol
 */
public class AdjSpreadPoisByPoiIdReducer extends Reducer<LongWritable,
        ProtobufWritable<PoiNew>, LongWritable, ProtobufWritable<PoiNew>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<PoiNew>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<PoiNew> value : values) {
            value.setConverter(PoiNew.class);
            final PoiNew poi = value.get();
            context.write(new LongWritable(poi.getId()), value);
        }
    }
}
