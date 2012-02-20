package es.tid.ps.mobility.jobs;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MxProtocol.MxCdr;

/**
 *
 * @author dmicol
 */
public class MobmxFilterCellnoinfoMapper extends Mapper<IntWritable,
        ProtobufWritable<MxCdr>, IntWritable, ProtobufWritable<MxCdr>> {
    @Override
    public void map(IntWritable key, ProtobufWritable<MxCdr> value,
            Context context) throws IOException, InterruptedException {
        MxCdr cdr = value.get();
        if (cdr.getCell() != 0) {
            context.write(new IntWritable(cdr.getCell()), value);
        } else {
            context.write(key, value);
        }
    }
}
