package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;

/**
 * Input: <Long, Cdr>
 * Output: <NodeBts, Null>
 * 
 * @author dmicol
 */
public class VectorSpreadNodedayhourReducer extends Reducer<LongWritable,
        MobilityWritable<Cdr>, ProtobufWritable<NodeBts>,
        MobilityWritable<Null>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Cdr>> values, Context context)
            throws IOException, InterruptedException {
        final MobilityWritable<Null> nullValue = new MobilityWritable<Null>(
                Null.getDefaultInstance());
        for (MobilityWritable<Cdr> value : values) {
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
            context.write(bts, nullValue);
        }
    }
}
