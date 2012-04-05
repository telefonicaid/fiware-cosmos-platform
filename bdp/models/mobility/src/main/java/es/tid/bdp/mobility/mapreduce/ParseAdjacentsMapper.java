package es.tid.bdp.mobility.mapreduce;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.mobility.data.MobProtocol.TwoInt;
import es.tid.bdp.mobility.parsing.AdjacentParser;
import java.io.IOException;
import org.apache.hadoop.io.NullWritable;

/**
 *
 * @author dmicol
 */
public class ParseAdjacentsMapper extends Mapper<IntWritable, Text,
        ProtobufWritable<TwoInt>, NullWritable> {
    @Override
    protected void map(IntWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        final TwoInt adjacent = new AdjacentParser(line.toString()).parse();
        ProtobufWritable wrappedAdjacent = ProtobufWritable.newInstance(
                TwoInt.class);
        wrappedAdjacent.set(adjacent);
        context.write(wrappedAdjacent, NullWritable.get());
    }
}
