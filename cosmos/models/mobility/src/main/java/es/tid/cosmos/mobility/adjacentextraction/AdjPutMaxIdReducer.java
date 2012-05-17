package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjPutMaxIdReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt pairPois = null;
        long max = Long.MIN_VALUE;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            pairPois = value.get().getTwoInt();
            if (pairPois.getNum2() > max) {
                max = pairPois.getNum2();
            }
        }
        context.write(key, MobDataUtil.createAndWrap(
                TwoIntUtil.create(pairPois.getNum1(), max)));
    }
}
