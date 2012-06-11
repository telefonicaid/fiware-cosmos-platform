package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Text>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class AdjParseAdjBtsMapper extends Mapper<LongWritable, Text,
        ProtobufWritable<TwoInt>, MobilityWritable<Null>> {
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final TwoInt adjBts = new AdjacentParser(value.toString()).parse();
            context.write(TwoIntUtil.wrap(adjBts),
                          new MobilityWritable(Null.getDefaultInstance()));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_ADJACENTS).increment(1L);
        }
    }
}
