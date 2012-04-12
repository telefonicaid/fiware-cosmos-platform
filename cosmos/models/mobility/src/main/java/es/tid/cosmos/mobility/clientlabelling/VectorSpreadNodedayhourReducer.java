package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;

/**
 *
 * @author dmicol
 */
public class VectorSpreadNodedayhourReducer extends Reducer<LongWritable,
        ProtobufWritable<Cdr>, ProtobufWritable<NodeBts>, NullWritable> {
    @Override
    protected void reduce(LongWritable key,
                          Iterable<ProtobufWritable<Cdr>> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        for (ProtobufWritable<Cdr> value : values) {
            final Cdr cdr = value.get();
            int weekday;
            switch (cdr.getDate().getWeekday()) {
                case 0:
                    weekday = 3;
                    break;
                case 5:
                    weekday = 1;
                    break;
                case 6:
                    weekday = 2;
                    break;
                default:
                    weekday = 0;
            }
            ProtobufWritable<NodeBts> bts = NodeBtsUtil.createAndWrap(
                    key.get(), 0, weekday, cdr.getTime().getHour());
            context.write(bts, NullWritable.get());
        }
    }
}
