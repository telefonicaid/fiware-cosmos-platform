package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Long, PoiNew>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        context.write(new LongWritable(nodBts.getNum1()), value);
    }
}
