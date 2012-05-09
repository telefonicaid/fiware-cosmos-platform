package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <Long, PoiNew>
 * 
 * @author dmicol
 */
public class AdjSpreadPoisByPoiIdMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final PoiNew poi = value.get().getPoiNew();
        context.write(new LongWritable(poi.getId()), value);
    }
}
