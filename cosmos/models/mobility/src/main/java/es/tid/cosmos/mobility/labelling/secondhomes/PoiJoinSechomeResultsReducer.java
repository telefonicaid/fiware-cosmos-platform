package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|TwoInt|Null>
 * Output: <TwoInt, Poi>
 * 
 * @author dmicol
 */
class PoiJoinSechomeResultsReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Poi poi = null;
        TwoInt ioweekIowend = null;
        boolean hasSecHomeCount = false;
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Poi) {
                if (poi == null) {
                    poi = (Poi)message;
                }
            } else if (message instanceof TwoInt) {
                if (ioweekIowend == null) {
                    ioweekIowend = (TwoInt)message;
                }
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
        context.write(key, new TypedProtobufWritable<Poi>(outputPoi.build()));
    }
}
