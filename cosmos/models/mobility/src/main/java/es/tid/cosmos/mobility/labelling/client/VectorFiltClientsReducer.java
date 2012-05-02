package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class VectorFiltClientsReducer extends Reducer<LongWritable,
        ProtobufWritable<Cdr>, LongWritable, ProtobufWritable<Cdr>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        int numComms = (int)context.getCounter(Counters.NUM_COMMS.name(),
                String.valueOf(key.get())).getValue();
        for (ProtobufWritable<Cdr> value : values) {
            for (int i = 0; i < numComms; i++) {
                context.write(key, value);
            }
        }
    }
}
