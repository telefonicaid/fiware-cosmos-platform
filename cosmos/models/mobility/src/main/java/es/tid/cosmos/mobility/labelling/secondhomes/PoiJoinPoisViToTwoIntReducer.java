package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, PoiPos|MobViMobVars>
 * Output: <TwoInt, TwoInt>
 * 
 * @author dmicol
 */
public class PoiJoinPoisViToTwoIntReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, PoiPos.class, MobViMobVars.class);
        List<PoiPos> poiPosList = dividedLists.get(PoiPos.class);
        List<MobViMobVars> mobVIVarsList = dividedLists.get(MobViMobVars.class);

        for (PoiPos poiPos : poiPosList) {
            PoiPos.Builder outputPoiPos = PoiPos.newBuilder(poiPos);
            for (MobViMobVars indvars : mobVIVarsList) {
                for (MobVars vars : indvars.getVarsList()) {
                    double distx = poiPos.getPosx() - vars.getMasscenterUtmx();
                    double disty = poiPos.getPosy() - vars.getMasscenterUtmy();
                    double dist = Math.sqrt(distx * distx + disty * disty);
                    if (vars.getWorkingday()) {
                        // Individual variables Monday - Friday
                        outputPoiPos.setInoutWeek(
                                dist <= vars.getRadius() ? 1 : 0);
                    } else {
                        // Individual variables Saturday - Sunday
                        outputPoiPos.setInoutWend(
                                dist <= vars.getRadius() ? 1 : 0);
                    }
                    outputPoiPos.setRadiusWeek(vars.getRadius());
                    outputPoiPos.setDistCMWeek(dist);
                }
                context.write(TwoIntUtil.createAndWrap(outputPoiPos.getNode(),
                                                       outputPoiPos.getBts()),
                              new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(
                                      outputPoiPos.getInoutWeek(),
                                      outputPoiPos.getInoutWend())));
            }
        }
    }
}
