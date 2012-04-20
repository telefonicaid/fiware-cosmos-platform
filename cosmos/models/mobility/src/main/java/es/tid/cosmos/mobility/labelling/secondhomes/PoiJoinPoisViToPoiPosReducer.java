package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;
import es.tid.cosmos.mobility.data.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.PoiPosUtil;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisViToPoiPosReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<PoiPos>> {
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
                    throw new IllegalStateException();
            }
        }

        for (PoiPos poiPos : poiPosList) {
            for (MobViMobVars indvars : mobVIVarsList) {
                PoiPos.Builder outputPoiPos = PoiPos.newBuilder(poiPos);
                for (MobVars vars : indvars.getVarsList()) {
                    double distx = poiPos.getPosx() - vars.getMasscenterUtmx();
                    double disty = poiPos.getPosy() - vars.getMasscenterUtmy();
                    double dist = Math.sqrt(distx * distx + disty * disty);

                    // Individual variables Monday - Friday
                    if (vars.getWorkingday()) {
                        outputPoiPos.setInoutWeek(
                                dist <= vars.getRadius() ? 1 : 0);
                        outputPoiPos.setRadiusWeek(vars.getRadius());
                        outputPoiPos.setDistCMWeek(dist);
                    }
                    // Individual variables Saturday - Sunday
                    if (!vars.getWorkingday()) {
                        outputPoiPos.setInoutWend(
                                dist <= vars.getRadius() ? 1 : 0);
                        outputPoiPos.setRadiusWend(vars.getRadius());
                        outputPoiPos.setDistCMWend(dist);
                    }
                }
                context.write(key, PoiPosUtil.wrap(outputPoiPos.build()));
            }
        }
    }
}
