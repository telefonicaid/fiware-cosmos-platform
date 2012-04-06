package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.MobProtocol.Cdr;
import es.tid.bdp.mobility.parsing.CdrParser;

/**
 *
 * @author sortega
 */
public class ParseCdrsMapper extends Mapper<IntWritable, Text, LongWritable,
        ProtobufWritable<Cdr>> {
    private LongWritable userId;
    
    @Override
    public void setup(Context context) {
        this.userId = new LongWritable();
    }
    
    @Override
    public void map(IntWritable key, Text line, Context context)
            throws IOException, InterruptedException {
        final Cdr cdr = new CdrParser(line.toString()).parse();
        ProtobufWritable wrappedCdr = ProtobufWritable.newInstance(Cdr.class);
        wrappedCdr.set(cdr);

        this.userId.set(cdr.getUserId());
        context.write(this.userId, wrappedCdr);
    }
}
