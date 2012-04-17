package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.util.CellCatalogue;

/**
 *
 * @author dmicol
 */
public class RepbtsJoinDistCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<NodeBtsDay>, LongWritable,
        ProtobufWritable<BtsCounter>> {
    private static final int MIN_TOTAL_CALLS = 200;
    private static final int MAX_TOTAL_CALLS = 5000;

    private CellCatalogue cellCatalogue;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.cellCatalogue = new CellCatalogue(context.getConfiguration());
    }

    @Override
    public void reduce(LongWritable key,
                       Iterable<ProtobufWritable<NodeBtsDay>> values,
                       Context context)
            throws IOException, InterruptedException {
        int numCommsInfo = 0;
        int numCommsNoInfo = 0;
        int numCommsNoBts = 0;
        for (ProtobufWritable<MobProtocol.NodeBtsDay> value : values) {
            value.setConverter(NodeBtsDay.class);
            final NodeBtsDay nodeBtsDay = value.get();
            if (!nodeBtsDay.hasPlaceId() || nodeBtsDay.getPlaceId() == 0) {
                numCommsNoInfo++;
            } else if (!this.cellCatalogue.contains(nodeBtsDay.getPlaceId())) {
                numCommsNoBts++;
            } else {
                numCommsInfo += nodeBtsDay.getCount();
            }
        }
        int totalComms = numCommsInfo + numCommsNoInfo + numCommsNoBts;
        if (totalComms >= MIN_TOTAL_CALLS && totalComms <= MAX_TOTAL_CALLS) {
            for (ProtobufWritable<NodeBtsDay> value : values) {
                final NodeBtsDay nodeBtsDay = value.get();
                ProtobufWritable<BtsCounter> counter =
                        BtsCounterUtil.createAndWrap(
                                nodeBtsDay.getPlaceId(),
                                0,
                                nodeBtsDay.getCount(),
                                nodeBtsDay.getCount() * 100 / totalComms);
                context.write(key, counter);
            }                                         
        }
    }
}
