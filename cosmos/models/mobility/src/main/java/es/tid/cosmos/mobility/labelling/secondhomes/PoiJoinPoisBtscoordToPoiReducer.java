package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiReducer extends Reducer<LongWritable,
        ProtobufWritable<Poi>, ProtobufWritable<TwoInt>,
        ProtobufWritable<Poi>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Poi> value : values) {
            value.setConverter(Poi.class);
            final Poi poi = value.get();
            context.write(TwoIntUtil.createAndWrap(poi.getNode(), poi.getBts()),
                          PoiUtil.wrap(poi));
        }
    }
}
