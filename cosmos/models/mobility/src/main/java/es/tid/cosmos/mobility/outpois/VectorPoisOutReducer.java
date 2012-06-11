package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class VectorPoisOutReducer extends Reducer<ProtobufWritable<TwoInt>,
        MobilityWritable<Poi>, NullWritable, Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<Poi> value : values) {
            final Poi poi = value.get();
            Poi.Builder poiBuilder = Poi.newBuilder(poi);
            poiBuilder.setLabelgroupnodebts(poi.getConfidentnodebts() == 0 ?
                    0 : poi.getLabelgroupnodebts());
            poiBuilder.setLabelgroupnode(poi.getConfidentnode() == 0 ?
                    0 : poi.getLabelgroupnode());
            poiBuilder.setLabelgroupbts(poi.getConfidentbts() == 0 ?
                    0 : poi.getLabelgroupbts());
            final Poi outputPoi = poiBuilder.build();
            String output =
                    outputPoi.getId() + PoiUtil.DELIMITER
                    + outputPoi.getNode() + PoiUtil.DELIMITER
                    + outputPoi.getBts() + PoiUtil.DELIMITER
                    + outputPoi.getLabelnodebts() + PoiUtil.DELIMITER
                    + outputPoi.getLabelgroupnodebts() + PoiUtil.DELIMITER
                    + outputPoi.getConfidentnodebts() + PoiUtil.DELIMITER
                    + outputPoi.getInoutWeek() + PoiUtil.DELIMITER
                    + outputPoi.getInoutWend() + PoiUtil.DELIMITER
                    + outputPoi.getLabelnode() + PoiUtil.DELIMITER
                    + outputPoi.getLabelgroupnode() + PoiUtil.DELIMITER
                    + outputPoi.getConfidentnode() + PoiUtil.DELIMITER
                    + outputPoi.getLabelbts() + PoiUtil.DELIMITER
                    + outputPoi.getLabelgroupbts() + PoiUtil.DELIMITER
                    + outputPoi.getConfidentbts();
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
