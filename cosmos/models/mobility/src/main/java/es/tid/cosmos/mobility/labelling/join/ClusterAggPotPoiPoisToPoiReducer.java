package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|Null>
 * Output: <Long, Poi>
 * 
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToPoiReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Message>, LongWritable,
        MobilityWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Poi poi = null;
        boolean hasNulls = false;
        for (MobilityWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Poi && poi == null) {
                poi =(Poi)message;
            } else if (message instanceof Null) {
                hasNulls = true;
            } else {
                throw new IllegalStateException("Unexpected input type: "
                        + message.getClass());
            }
            if (poi != null && hasNulls) {
                break;
            }
        }
        if (poi == null) {
            return;
        }
        Poi.Builder outputPoi = Poi.newBuilder(poi);
        if (hasNulls) {
            outputPoi.setConfidentnodebts(1);
        }
        context.write(new LongWritable(outputPoi.getBts()),
                      new MobilityWritable<Poi>(outputPoi.build()));
    }
}
