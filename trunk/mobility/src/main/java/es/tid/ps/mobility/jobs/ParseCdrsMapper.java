package es.tid.ps.mobility.jobs;

import es.tid.ps.mobility.parser.CdrParser;
import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MxProtocol.MxCdr;

/**
 *
 * @author sortega
 */
public class ParseCdrsMapper extends Mapper<IntWritable, Text, LongWritable,
        ProtobufWritable<MxCdr>> {

    @Override
    protected void map(IntWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        MxCdr cdr = new CdrParser(line.toString()).parse();
        ProtobufWritable wrappedCdr = ProtobufWritable.newInstance(MxCdr.class);
        wrappedCdr.set(cdr);
        context.write(new LongWritable(cdr.getPhone()), wrappedCdr);
    }
}
