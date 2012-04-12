package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author sortega
 */
public class ParseCdrsMapper extends Mapper<LongWritable, Text, LongWritable,
        ProtobufWritable<Cdr>> {
    private LongWritable userId;
    
    @Override
    public void setup(Context context) {
        this.userId = new LongWritable();
    }
    
    @Override
    public void map(LongWritable key, Text line, Context context)
            throws IOException, InterruptedException {
        ProtobufWritable<Cdr> cdr = CdrUtil.wrap(CdrUtil.parse(line.toString()));
        this.userId.set(cdr.get().getUserId());
        context.write(this.userId, cdr);
    }
}
