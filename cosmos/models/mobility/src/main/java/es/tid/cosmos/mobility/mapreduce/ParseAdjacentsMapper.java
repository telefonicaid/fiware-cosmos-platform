package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.parsing.AdjacentParser;

/**
 *
 * @author dmicol
 */
public class ParseAdjacentsMapper extends Mapper<IntWritable, Text,
        ProtobufWritable<TwoInt>, NullWritable> {
    @Override
    public void map(IntWritable lineno, Text line, Context context)
            throws IOException, InterruptedException {
        final TwoInt adjacent = new AdjacentParser(line.toString()).parse();
        ProtobufWritable wrappedAdjacent = ProtobufWritable.newInstance(
                TwoInt.class);
        wrappedAdjacent.set(adjacent);
        context.write(wrappedAdjacent, NullWritable.get());
    }
}
