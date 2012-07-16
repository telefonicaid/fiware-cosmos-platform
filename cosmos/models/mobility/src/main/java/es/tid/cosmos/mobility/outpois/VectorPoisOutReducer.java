package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <Null, Text>
 *
 * @author dmicol
 */
class VectorPoisOutReducer extends Reducer<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Poi>, NullWritable, Text> {
    private String separator;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }

    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<Poi> value : values) {
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
                    outputPoi.getId() + this.separator
                    + outputPoi.getNode() + this.separator
                    + outputPoi.getBts() + this.separator
                    + outputPoi.getLabelnodebts() + this.separator
                    + outputPoi.getLabelgroupnodebts() + this.separator
                    + outputPoi.getConfidentnodebts() + this.separator
                    + outputPoi.getInoutWeek() + this.separator
                    + outputPoi.getInoutWend() + this.separator
                    + outputPoi.getLabelnode() + this.separator
                    + outputPoi.getLabelgroupnode() + this.separator
                    + outputPoi.getConfidentnode() + this.separator
                    + outputPoi.getLabelbts() + this.separator
                    + outputPoi.getLabelgroupbts() + this.separator
                    + outputPoi.getConfidentbts();
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
