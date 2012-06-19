package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Input: <Long, Int|Cdr>
 * Output: <Long, Cdr>
 * 
 * @author dmicol
 */
class VectorFiltClientsReducer extends Reducer<
        LongWritable, TypedProtobufWritable<Message>,
        LongWritable, TypedProtobufWritable<Cdr>> {
    private int maxTotalCalls;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.maxTotalCalls = conf.getClientMaxTotalCalls();
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        boolean hasComms = false;
        List<Cdr> cdrList = new LinkedList<Cdr>();
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Int) {
                hasComms = true;
            } else if (message instanceof Cdr) {
                cdrList.add((Cdr)message);
            } else {
                throw new IllegalArgumentException("Invalid input type: "
                        + message.getClass());
            }
            
            if (hasComms) {
                break;
            }
            if (cdrList.size() > this.maxTotalCalls) {
                return;
            }
        }
        if (!hasComms) {
            // If we don't have communications for this user, we won't output
            // any CDRs
            return;
        }
        for (Cdr cdr : cdrList) {
            context.write(key, new TypedProtobufWritable<Cdr>(cdr));
        }
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (!(message instanceof Cdr)) {
                throw new IllegalStateException("Invalid input type: "
                        + message.getClass());
            }
            context.write(key, new TypedProtobufWritable<Cdr>((Cdr) message));
        }
    }
}
