package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input:  <Long, TwoInt>
 * Output: <Long, Long>
 *
 * @author dmicol
 */
class AdjSpreadTableByPoiIdMapper extends Mapper<LongWritable,
        TypedProtobufWritable<TwoInt>, LongWritable, TypedProtobufWritable<Int64>> {
    @Override
    protected void map(LongWritable key,
            TypedProtobufWritable<TwoInt> value, Context context)
            throws IOException, InterruptedException {
        final TwoInt poiPoiMod = value.get();
        context.write(new LongWritable(poiPoiMod.getNum1()),
                      TypedProtobufWritable.create(poiPoiMod.getNum2()));
    }
}
