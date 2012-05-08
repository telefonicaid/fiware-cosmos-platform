package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Long>
 * 
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void map(LongWritable key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final TwoInt poiPoiMod = value.get().getTwoInt();
        context.write(new LongWritable(poiPoiMod.getNum1()),
                      MobDataUtil.createAndWrap(poiPoiMod.getNum2()));
    }
}
