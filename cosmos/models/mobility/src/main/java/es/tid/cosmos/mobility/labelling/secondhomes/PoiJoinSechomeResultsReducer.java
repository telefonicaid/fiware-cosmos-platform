package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|TwoInt|Null>
 * Output: <TwoInt, Poi>
 * 
 * @author dmicol
 */
public class PoiJoinSechomeResultsReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Message>,
        ProtobufWritable<TwoInt>, MobilityWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Poi poi = null;
        TwoInt ioweekIowend = null;
        boolean hasSecHomeCount = false;
        for (MobilityWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Poi && poi == null) {
                poi = (Poi)message;
            } else if (message instanceof TwoInt && ioweekIowend == null) {
                ioweekIowend = (TwoInt)message;
            } else if (message instanceof Null) {
                hasSecHomeCount = true;
            } else {
                throw new IllegalStateException("Unexpected input type: "
                        + message.getClass());
            }
            
            if (poi != null && ioweekIowend != null && hasSecHomeCount) {
                // We already have all the required values, so there's no need
                // to process more records
                break;
            }
        }
        
        Poi.Builder outputPoi = Poi.newBuilder(poi);
        outputPoi.setInoutWeek((int)ioweekIowend.getNum1());
        outputPoi.setInoutWend((int)ioweekIowend.getNum2());
        if (hasSecHomeCount) {
            outputPoi.setLabelnodebts(100);
            outputPoi.setLabelgroupnodebts(100);
        }
        context.write(key, new MobilityWritable<Poi>(outputPoi.build()));
    }
}
