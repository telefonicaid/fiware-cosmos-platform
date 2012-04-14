package es.tid.cosmos.mobility.preparing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class FilterCellnoinfoByCellIdMapper extends Mapper<LongWritable,
        ProtobufWritable<Cdr>, LongWritable, ProtobufWritable<Cdr>> {
    @Override
    public void map(LongWritable key, ProtobufWritable<Cdr> value,
                    Context context) throws IOException, InterruptedException {
        final Cdr cdr = value.get();
        if (cdr.getCellId() != 0) {
            context.write(new LongWritable(cdr.getCellId()), value);
        }
    }
}
