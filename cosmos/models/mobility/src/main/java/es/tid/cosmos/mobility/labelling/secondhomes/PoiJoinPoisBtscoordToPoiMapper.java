package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiMapper extends Mapper<LongWritable,
        ProtobufWritable<Poi>, ProtobufWritable<TwoInt>,
        ProtobufWritable<Poi>> {
    @Override
    protected void map(LongWritable key, ProtobufWritable<Poi> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(Poi.class);
        final Poi poi = value.get();
        context.write(TwoIntUtil.createAndWrap(poi.getNode(), poi.getBts()),
                        PoiUtil.wrap(poi));
    }
}
