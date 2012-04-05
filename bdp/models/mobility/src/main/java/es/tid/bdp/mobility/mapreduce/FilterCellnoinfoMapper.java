package es.tid.bdp.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class FilterCellnoinfoMapper extends Mapper<LongWritable,
        ProtobufWritable<Cdr>, LongWritable, ProtobufWritable<Cdr>> {
    public enum OutputKey {
        INVALID,
        CELL,
        NODE
    }

    private OutputKey outputKey;

    public FilterCellnoinfoMapper() {
        this.outputKey = OutputKey.INVALID;
    }

    @Override
    public void map(LongWritable key, ProtobufWritable<Cdr> value,
            Context context) throws IOException, InterruptedException {
        this.outputKey = OutputKey.valueOf(context.getConfiguration().
                get("outputKey"));

        final Cdr cdr = value.get();
        if (cdr.getCellId() != 0) {
            context.write(new LongWritable(cdr.getCellId()), value);
        } else {
            switch (this.outputKey) {
                case CELL:
                    context.write(new LongWritable(cdr.getCellId()), value);
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
