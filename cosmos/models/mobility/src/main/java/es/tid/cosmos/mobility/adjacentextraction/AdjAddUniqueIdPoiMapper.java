package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Long, Poi>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiMapper extends Mapper<
        ProtobufWritable<TwoInt>, MobilityWritable<Poi>,
        LongWritable, MobilityWritable<Poi>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            MobilityWritable<Poi> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        context.write(new LongWritable(nodBts.getNum1()), value);
    }
}
