package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class ClusterSpreadNodelblPoilblReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Poi>,
        ProtobufWritable<TwoInt>, MobilityWritable<Null>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<Poi> value : values) {
            final Poi poi = value.get();
            if (poi.getConfidentnode() == 1 && poi.getConfidentnodebts() == 1) {
                context.write(TwoIntUtil.createAndWrap(poi.getLabelnode(),
                                                       poi.getLabelnodebts()),
                              new MobilityWritable<Null>(
                                      Null.getDefaultInstance()));
            }
        }
    }
}
