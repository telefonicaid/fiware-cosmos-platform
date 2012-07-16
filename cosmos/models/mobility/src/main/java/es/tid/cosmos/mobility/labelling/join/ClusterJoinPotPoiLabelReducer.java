package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi|Long>
 * Output: <TwoInt, Null>
 *
 * @author dmicol
 */
class ClusterJoinPotPoiLabelReducer extends Reducer<
        LongWritable, TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Null>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, Poi.class, Int64.class);
        List<Poi> poiList = dividedLists.get(Poi.class);
        List<Int64> longList = dividedLists.get(Int64.class);

        for (Int64 majPoiInt64 : longList) {
            final long majPoiLbl = majPoiInt64.getValue();
            for (Poi potPoi : poiList) {
                if (majPoiLbl == potPoi.getLabelnodebts()) {
                    context.write(TwoIntUtil.createAndWrap(potPoi.getNode(),
                                                           potPoi.getBts()),
                                  new TypedProtobufWritable<Null>(
                                          Null.getDefaultInstance()));
                }
            }
        }
    }
}
