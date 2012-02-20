package es.tid.ps.mobility.jobs;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MxProtocol.MxCdr;

/**
 *
 * @author dmicol
 */
public class MobmxFilterCellnoinfoMapper extends Mapper<LongWritable,
        ProtobufWritable<MxCdr>, LongWritable, ProtobufWritable<MxCdr>> {
    @Override
    public void map(LongWritable key, ProtobufWritable<MxCdr> value,
            Context context) throws IOException, InterruptedException {
        MxCdr cdr = value.get();
        if (cdr.getCell() != 0) {
            context.write(new LongWritable(cdr.getCell()), value);
        } else {
            context.write(key, value);
        }
    }
}
