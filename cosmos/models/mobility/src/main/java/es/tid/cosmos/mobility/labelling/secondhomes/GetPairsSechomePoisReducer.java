package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, PoiPos>
 * Output: <TwoInt, Long>
 * 
 * @author dmicol
 */
public class GetPairsSechomePoisReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiPos> poiPosList = new LinkedList<PoiPos>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            poiPosList.add(value.get().getPoiPos());
        }

        for (PoiPos poiIn : poiPosList) {
            if (poiIn.getInoutWeek() == 1 &&
                    (poiIn.getLabel() == Config.homeLabelgroupId ||
                     poiIn.getLabel() == Config.workLabelgroupId)) {
                for (PoiPos poiOut : poiPosList) {
                    if (poiOut.getInoutWeek() == 0 &&
                            (poiOut.getLabel() == Config.homeLabelgroupId ||
                             poiOut.getLabel() == Config.workLabelgroupId)) {
                        double distx = poiIn.getPosx() - poiOut.getPosx();
                        double disty = poiIn.getPosy() - poiOut.getPosy();
                        double dist = Math.sqrt(distx * distx + disty * disty);
                        if (dist >= Config.minDistSecondHome) {
                            context.write(
                                    TwoIntUtil.createAndWrap(poiIn.getBts(),
                                                             poiOut.getBts()),
                                    MobDataUtil.createAndWrap(poiIn.getNode()));
                        }
                    }
                }
            }
        }
    }
}
