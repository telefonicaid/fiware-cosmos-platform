package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <Long, Cdr|NodeBsDay>
 * Output: <Long, Int>
 * 
 * @author dmicol
 */
class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<Int>> {
    private int minTotalCalls;
    private int maxTotalCalls;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.minTotalCalls = conf.getClientMinTotalCalls();
        this.maxTotalCalls = conf.getClientMaxTotalCalls();
    }

    @Override
    public void reduce(LongWritable key,
                       Iterable<TypedProtobufWritable<Message>> values,
                       Context context)
            throws IOException, InterruptedException {
        int numCommsInfo = 0;
        int numCommsNoInfoOrNoBts = 0;
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Cdr) {
                numCommsNoInfoOrNoBts++;
            } else if (message instanceof NodeBtsDay) {
                final NodeBtsDay nodeBtsDay = (NodeBtsDay) message;
                numCommsInfo += nodeBtsDay.getCount();
            } else {
                throw new IllegalArgumentException();
            }
        }
        int totalComms = numCommsInfo + numCommsNoInfoOrNoBts;
        if (totalComms >= this.minTotalCalls &&
                totalComms <= this.maxTotalCalls) {
            context.write(key, TypedProtobufWritable.create(numCommsInfo));
        }
    }
}
