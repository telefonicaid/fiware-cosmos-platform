package es.tid.ps.mobility.mapreduce;

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
    public enum OutputKey {
        INVALID,
        CELL,
        NODE
    }

    private OutputKey outputKey;

    public MobmxFilterCellnoinfoMapper() {
        this.outputKey = OutputKey.INVALID;
    }

    public void setOutputKey(OutputKey key) {
        this.outputKey = key;
    }

    @Override
    public void map(LongWritable key, ProtobufWritable<MxCdr> value,
            Context context) throws IOException, InterruptedException {
        MxCdr cdr = value.get();
        if (cdr.getCell() != 0) {
            context.write(new LongWritable(cdr.getCell()), value);
        } else {
            switch (this.outputKey) {
                case CELL:
                    context.write(new LongWritable(cdr.getCell()), value);
                    break;
                case NODE:
                    context.write(key, value);
                    break;
                default:
                    throw new RuntimeException("Output key type is not valid.");
            }
        }
    }
}
