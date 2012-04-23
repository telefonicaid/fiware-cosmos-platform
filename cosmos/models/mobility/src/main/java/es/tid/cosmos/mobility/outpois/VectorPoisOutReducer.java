package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.PoiUtil;

/**
 *
 * @author dmicol
 */
public class VectorPoisOutReducer extends Reducer<NullWritable,
        ProtobufWritable<Poi>, NullWritable, Text> {
    @Override
    protected void reduce(NullWritable key,
            Iterable<ProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Poi> value : values) {
            value.setConverter(Poi.class);
            final Poi poi = value.get();
            Poi.Builder poiBuilder = Poi.newBuilder(poi);
            poiBuilder.setLabelgroupnodebts(poi.getConfidentnodebts() == 0 ?
                    0 : poi.getLabelgroupnodebts());
            poiBuilder.setLabelgroupnode(poi.getConfidentnode() == 0 ?
                    0 : poi.getLabelgroupnode());
            poiBuilder.setLabelgroupbts(poi.getConfidentbts() == 0 ?
                    0 : poi.getLabelgroupbts());
            context.write(NullWritable.get(),
                          new Text(PoiUtil.toString(poiBuilder.build())));
        }
    }
}
