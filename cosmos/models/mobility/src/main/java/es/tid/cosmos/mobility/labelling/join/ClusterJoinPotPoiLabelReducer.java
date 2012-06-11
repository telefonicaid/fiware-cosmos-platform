package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi|Long>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class ClusterJoinPotPoiLabelReducer extends Reducer<
        LongWritable, MobilityWritable<Message>, ProtobufWritable<TwoInt>,
        MobilityWritable<Null>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = MobilityWritable.divideIntoTypes(
                values, Poi.class, Int64.class);
        List<Poi> poiList = dividedLists.get(Poi.class);
        List<Long> longList = dividedLists.get(Long.class);
        
        for (Long majPoiLbl : longList) {
            for (Poi potPoi : poiList) {
                if (majPoiLbl == potPoi.getLabelnodebts()) {
                    context.write(TwoIntUtil.createAndWrap(potPoi.getNode(),
                                                           potPoi.getBts()),
                                  new MobilityWritable<Null>(
                                          Null.getDefaultInstance()));
                }
            }
        }
    }
}
