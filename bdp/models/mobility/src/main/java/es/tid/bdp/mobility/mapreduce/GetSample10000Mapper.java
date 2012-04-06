package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class GetSample10000Mapper extends Mapper<IntWritable,
        ProtobufWritable<Cdr>, LongWritable, ProtobufWritable<Cdr>> {
    private LongWritable key;
    
    @Override
    protected void setup(final Context context) {
        this.key = new LongWritable();
    }
    
    @Override
    public void map(IntWritable key, ProtobufWritable<Cdr> value,
                    Context context) throws IOException, InterruptedException {
        final Cdr cdr = value.get();
        // TODO: do not use hard-coded numbers.
        int nod = (key.get() % 100000) / 1000;
        if (nod == 92) {
            this.key.set(cdr.getUserId());
            context.write(this.key, value);
        }
    }
}
