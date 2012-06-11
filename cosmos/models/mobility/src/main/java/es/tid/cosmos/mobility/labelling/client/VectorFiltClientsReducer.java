package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;

/**
 * Input: <Long, Int|Cdr>
 * Output: <Long, Cdr>
 * 
 * @author dmicol
 */
public class VectorFiltClientsReducer extends Reducer<LongWritable,
        MobilityWritable<Message>, LongWritable, MobilityWritable<Cdr>> {
    private int maxCdrs;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.maxCdrs = conf.getInt(Config.MAX_CDRS, Integer.MAX_VALUE);
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        boolean hasComms = false;
        List<Cdr> cdrList = new LinkedList<Cdr>();
        for (MobilityWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Int) {
                hasComms = true;
            } else if (message instanceof Cdr) {
                cdrList.add((Cdr)message);
            } else {
                throw new IllegalArgumentException();
            }
            
            if (hasComms) {
                break;
            }
            if (cdrList.size() > this.maxCdrs) {
                return;
            }
        }
        if (!hasComms) {
            // If we don't have communications for this user, we won't output
            // any CDRs
            return;
        }
        for (Cdr cdr : cdrList) {
            context.write(key, new MobilityWritable<Cdr>(cdr));
        }
        for (MobilityWritable<Message> value : values) {
            final Cdr cdr = (Cdr) value.get();
            if (cdr == null) {
                throw new IllegalStateException();
            }
            context.write(key, new MobilityWritable<Cdr>(cdr));
        }
    }
}
