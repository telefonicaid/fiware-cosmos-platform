package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiPosUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiPos;

/**
 * Input: <Long, Poi|Cell>
 * Output: <Long, PoiPos>
 * 
 * @author dmicol
 */
public class PoiJoinPoisBtscoordToPoiPosReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<PoiPos>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        Cell cell = null;
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Poi) {
                poiList.add((Poi)message);
            } else if (message instanceof Cell) {
                if (cell == null) {
                    cell = (Cell) message;
                }
            } else {
                throw new IllegalStateException("Unexpected input type: "
                        + message.getClass());
            }
        }
        
        for (Poi poi : poiList) {
            PoiPos poiPos = PoiPosUtil.create(
                    poi.getNode(), poi.getBts(),
                    poi.getConfidentnodebts() == 0 ? 0 :
                                                     poi.getLabelgroupnodebts(),
                    cell.getPosx(), cell.getPosy(), poi.getInoutWeek(),
                    poi.getInoutWend(), -1D, -1D, -1D, -1D);
            context.write(new LongWritable(poi.getNode()),
                          new TypedProtobufWritable<PoiPos>(poiPos));
        }
    }
}
