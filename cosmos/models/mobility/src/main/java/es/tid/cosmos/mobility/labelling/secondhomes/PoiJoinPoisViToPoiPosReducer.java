package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 * Input: <Long, PoiPos|MobViMobVars>
 * Output: <Long, PoiPos>
 * 
 * @author dmicol
 */
public class PoiJoinPoisViToPoiPosReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiPos> poiPosList = new LinkedList<PoiPos>();
        List<MobViMobVars> mobVIVarsList = new LinkedList<MobViMobVars>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI_POS:
                    poiPosList.add(mobData.getPoiPos());
                    break;
                case MOB_VI_MOB_VARS:
                    mobVIVarsList.add(mobData.getMobViMobVars());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }

        for (PoiPos poiPos : poiPosList) {
            for (MobViMobVars indvars : mobVIVarsList) {
                PoiPos.Builder outputPoiPos = PoiPos.newBuilder(poiPos);
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
                context.write(key,
                              MobDataUtil.createAndWrap(outputPoiPos.build()));
            }
        }
    }
}
