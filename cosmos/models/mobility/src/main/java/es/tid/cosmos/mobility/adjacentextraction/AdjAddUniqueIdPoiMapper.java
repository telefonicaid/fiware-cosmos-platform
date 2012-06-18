package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Long, Poi>
 * 
 * @author dmicol
 */
class AdjAddUniqueIdPoiMapper extends Mapper<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
        LongWritable, TypedProtobufWritable<Poi>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            TypedProtobufWritable<Poi> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        context.write(new LongWritable(nodBts.getNum1()), value);
    }
}
