package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Long|PoiNew>
 * Output: <TwoInt, PoiNew>
 *
 * @author dmicol
 */
class AdjJoinNewPoiIdReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<PoiNew>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedValues = TypedProtobufWritable.groupByClass(
                values, Int64.class, PoiNew.class);
        List<Int64> longList = dividedValues.get(Int64.class);
        List<PoiNew> poiNewList = dividedValues.get(PoiNew.class);

        for (Int64 poiMod : longList) {
            int poiModVal = (int) poiMod.getValue();
            for (PoiNew poi : poiNewList) {
                PoiNew.Builder outputPoiBuilder = PoiNew.newBuilder(poi);
                outputPoiBuilder.setId(poiModVal);
                context.write(TwoIntUtil.createAndWrap(poi.getNode(),
                                                       poi.getBts()),
                              new TypedProtobufWritable<PoiNew>(
                                      outputPoiBuilder.build()));
            }
        }
    }
}
