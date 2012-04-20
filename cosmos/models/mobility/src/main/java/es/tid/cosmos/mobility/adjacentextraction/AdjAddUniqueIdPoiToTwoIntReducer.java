package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<Poi>, LongWritable,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        int hash = (int)TwoIntUtil.getPartition(nodBts, 100L);
        int counter = 0;
        for (ProtobufWritable<Poi> value : values) {
            value.setConverter(Poi.class);
            int tableId = (100 * counter++) + hash;
            ProtobufWritable<TwoInt> poiPoimod = TwoIntUtil.createAndWrap(
                    tableId, tableId);
            context.write(new LongWritable(tableId), poiPoimod);
        }
    }
}
