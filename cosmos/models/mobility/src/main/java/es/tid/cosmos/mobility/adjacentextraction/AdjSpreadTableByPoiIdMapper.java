package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapper extends Mapper<LongWritable,
        ProtobufWritable<TwoInt>, LongWritable, LongWritable> {
    @Override
    protected void map(LongWritable key,
            ProtobufWritable<TwoInt> value, Context context)
            throws IOException, InterruptedException {
        value.setConverter(TwoInt.class);
        final TwoInt poiPoiMod = value.get();
        context.write(new LongWritable(poiPoiMod.getNum1()),
                      new LongWritable(poiPoiMod.getNum2()));
    }
}
