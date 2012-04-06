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

    private OutputKey outputType;
    private LongWritable outputKey;

    @Override
    public void setup(Context context) {
        this.outputType = OutputKey.INVALID;
        this.outputKey = new LongWritable();
    }

    @Override
    public void map(LongWritable key, ProtobufWritable<Cdr> value,
                    Context context) throws IOException, InterruptedException {
        final Cdr cdr = value.get();
        if (cdr.getCellId() != 0) {
            this.outputKey.set(cdr.getCellId());
        } else {
            this.outputType = OutputKey.valueOf(context.getConfiguration().
                    get("outputKey"));
            switch (this.outputType) {
                case CELL:
                    this.outputKey.set(cdr.getCellId());
                    break;
                case NODE:
                    this.outputKey.set(key.get());
                    break;
                default:
                    throw new RuntimeException("Output key type is not valid.");
            }
        }
        context.write(key, value);
    }
}
