package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.PoiPos;
import es.tid.cosmos.mobility.data.PoiPosUtil;

/**
 *
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiPosReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<PoiPos>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        List<Cell> cellList = new LinkedList<Cell>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                case CELL:
                    cellList.add(mobData.getCell());
                    break;
                default:
                    throw new IllegalStateException();
            }
        }
        
        for (Poi poi : poiList) {
            Cell cell = cellList.get(0);
            ProtobufWritable<PoiPos> poiPos = PoiPosUtil.createAndWrap(
                    poi.getNode(), poi.getBts(),
                    poi.getConfidentnodebts() == 0 ? 0 :
                                                     poi.getLabelgroupnodebts(),
                    cell.getPosx(), cell.getPosy(), poi.getInoutWeek(),
                    poi.getInoutWend(), -1D, -1D, -1D, -1D);
            context.write(new LongWritable(poi.getNode()), poiPos);
        }
    }
}
