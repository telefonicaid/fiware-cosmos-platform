package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdReducer extends Reducer<LongWritable,
        ProtobufWritable<TwoInt>, LongWritable, LongWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<TwoInt> value : values) {
            value.setConverter(TwoInt.class);
            final TwoInt poiPoiMod = value.get();
            context.write(new LongWritable(poiPoiMod.getNum1()),
                          new LongWritable(poiPoiMod.getNum2()));
        }
    }
}
