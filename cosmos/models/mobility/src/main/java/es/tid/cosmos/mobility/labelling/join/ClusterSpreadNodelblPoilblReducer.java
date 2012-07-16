package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <TwoInt, Null>
 *
 * @author dmicol
 */
class ClusterSpreadNodelblPoilblReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<Poi> value : values) {
            final Poi poi = value.get();
            if (poi.getConfidentnode() == 1 && poi.getConfidentnodebts() == 1) {
                context.write(TwoIntUtil.createAndWrap(poi.getLabelnode(),
                                                       poi.getLabelnodebts()),
                              new TypedProtobufWritable<Null>(
                                      Null.getDefaultInstance()));
            }
        }
    }
}
