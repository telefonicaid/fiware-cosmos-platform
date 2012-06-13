package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi>
 * Output: <TwoInt, Poi>
 * 
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiMapper extends Mapper<LongWritable,
        TypedProtobufWritable<Poi>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Poi>> {
    @Override
    protected void map(LongWritable key, TypedProtobufWritable<Poi> value,
            Context context) throws IOException, InterruptedException {
        final Poi poi = value.get();
        context.write(TwoIntUtil.createAndWrap(poi.getNode(), poi.getBts()),
                      value);
    }
}
