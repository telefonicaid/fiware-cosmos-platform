package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <Long, PoiPos>
 * Output: <TwoInt, Long>
 * 
 * @author dmicol
 */
public class GetPairsSechomePoisReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    private static final int HOME_LABELGROUP_ID = 3;
    private static final int WORK_LABELGROUP_ID = 6;
    private static final double MIN_DIST_SECOND_HOME = 49342.85D;
    
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
                    (poiIn.getLabel() == HOME_LABELGROUP_ID ||
                     poiIn.getLabel() == WORK_LABELGROUP_ID)) {
                for (PoiPos poiOut : poiPosList) {
                    if (poiOut.getInoutWeek() == 0 &&
                            (poiOut.getLabel() == HOME_LABELGROUP_ID ||
                             poiOut.getLabel() == WORK_LABELGROUP_ID)) {
                        double distx = poiIn.getPosx() - poiOut.getPosx();
                        double disty = poiIn.getPosy() - poiOut.getPosy();
                        double dist = Math.sqrt(distx * distx + disty * disty);
                        if (dist >= MIN_DIST_SECOND_HOME) {
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
