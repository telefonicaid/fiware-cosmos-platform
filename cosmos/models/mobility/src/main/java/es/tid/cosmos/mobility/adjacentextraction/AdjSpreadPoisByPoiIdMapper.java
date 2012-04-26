package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadPoisByPoiIdMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<PoiNew>,
        LongWritable, ProtobufWritable<PoiNew>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<PoiNew> value, Context context)
            throws IOException, InterruptedException {
        value.setConverter(PoiNew.class);
        final PoiNew poi = value.get();
        context.write(new LongWritable(poi.getId()), value);
    }
}
