package es.tid.ps.mobility.jobs;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.ps.mobility.data.MxProtocol.MxCdr;
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

/**
 *
 * @author dmicol
 */
public class NobmxGetSample10000Mapper extends Mapper<IntWritable,
        ProtobufWritable<MxCdr>, IntWritable, ProtobufWritable<MxCdr>> {
    @Override
    public void map(IntWritable key, ProtobufWritable<MxCdr> value,
            Context context) throws IOException, InterruptedException {
        MxCdr cdr = value.get();
        // TODO: do not use hard-coded numbers.
        int nod = (key.get() % 100000) / 1000;
        if (nod == 92) {
            context.write(new IntWritable(cdr.getPhone()), value);
        }
    }
}
