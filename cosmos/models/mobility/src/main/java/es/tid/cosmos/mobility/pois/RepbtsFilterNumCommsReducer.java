package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <Long, Cdr|NodeBsDay>
 * Output: <Long, Int>
 * 
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        MobilityWritable<Message>, LongWritable, MobilityWritable<Int>> {
    private int minTotalCalls;
    private int maxTotalCalls;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.minTotalCalls = conf.getInt(Config.MIN_TOTAL_CALLS,
                                         Integer.MIN_VALUE);
        this.maxTotalCalls = conf.getInt(Config.MAX_TOTAL_CALLS,
                                         Integer.MAX_VALUE);
    }

    @Override
    public void reduce(LongWritable key,
                       Iterable<MobilityWritable<Message>> values,
                       Context context)
            throws IOException, InterruptedException {
        int numCommsInfo = 0;
        int numCommsNoInfoOrNoBts = 0;
        for (MobilityWritable<Message> value : values) {
            final Message message = value.get();
            if(message instanceof Cdr) {
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
            context.write(key, MobilityWritable.create(numCommsInfo));
        }
    }
}
